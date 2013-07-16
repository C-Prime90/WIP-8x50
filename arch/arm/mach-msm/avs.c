/*
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/kernel_stat.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/kthread.h>

#ifdef CONFIG_CPU_FREQ_VDD_LEVELS
#include "board-bravo.h"
#endif

#include "avs.h"

#define AVSDSCR_INPUT 0x01004860 /* magic # from circuit designer */
#define TSCSR_INPUT   0x00000001 /* enable temperature sense */

#define TEMPRS 16                /* total number of temperature regions */
#define GET_TEMPR() (avs_get_tscsr() >> 28) /* scale TSCSR[CTEMP] to regions */

struct mutex avs_lock;

static struct avs_state_s
{
	u32 freq_cnt;		/* Frequencies supported list */
	short *avs_v;		/* Dyanmically allocated storage for
				 * 2D table of voltages over temp &
				 * freq.  Used as a set of 1D tables.
				 * Each table is for a single temp.
				 * For usage see avs_get_voltage
				 */
	int (*set_vdd) (int);	/* Function Ptr for setting voltage */
	int changing;		/* Clock frequency is changing */
	u32 freq_idx;		/* Current frequency index */
	int vdd;		/* Current ACPU voltage */
} avs_state;

struct clkctl_acpu_speed {
  unsigned	acpu_khz;
  int		min_vdd;
  int		max_vdd;
  int		max_vdd_hard_limit;	// Voltage that must never be beaten
  unsigned	ignore;	// Set to 1 if this frequency is to be ignored
};

#ifndef MAX
# define	MAX(A,B)	(A>B?A:B)
#endif // !def MAX
#ifndef MIN
# define	MIN(A,B)	(A<B?A:B)
#endif // !def MIN

struct clkctl_acpu_speed acpu_vdd_tbl[] = {
  {  19200, VOLTAGE_MIN_START, VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 176000, VOLTAGE_MIN_START, VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 0 },
  { 245000, VOLTAGE_MIN_START, VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 0 },
  { 406000, VOLTAGE_MIN_START, VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 446000, VOLTAGE_MIN_START, VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 506000, MAX(VOLTAGE_MIN_START,1075), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 0 },
  { 554000, MAX(VOLTAGE_MIN_START,1100), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 609000, MAX(VOLTAGE_MIN_START,1125), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 703000, MAX(VOLTAGE_MIN_START,1200), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 760000, MAX(VOLTAGE_MIN_START,1200), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 0 },
  { 859000, MAX(VOLTAGE_MIN_START,1250), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 0 },
  { 902000, MAX(VOLTAGE_MIN_START,1275), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 1 },
  { 957000, MAX(VOLTAGE_MIN_START,1275), VOLTAGE_MAX_SAFE, VOLTAGE_STABLE, 0 },
  { 1002000, MAX(VOLTAGE_MIN_START,1275), VOLTAGE_STABLE, VOLTAGE_STABLE, 0 },
  { 1057000, MAX(VOLTAGE_MIN_START,1275), VOLTAGE_STABLE, VOLTAGE_STABLE, 1 },
  { 1106000, MAX(VOLTAGE_MIN_START,1275), VOLTAGE_STABLE, VOLTAGE_STABLE, 0 },
  { 1153000, MAX(VOLTAGE_MIN_START,1300), VOLTAGE_STABLE, VOLTAGE_STABLE, 1 },
  { 1206000, MAX(VOLTAGE_MIN_START,1300), VOLTAGE_STABLE, VOLTAGE_STABLE, 1 },
  { 1253000, MAX(VOLTAGE_MIN_START,1300), VOLTAGE_STABLE, VOLTAGE_STABLE, 0 },
  { 1304000, MAX(VOLTAGE_MIN_START,1300), VOLTAGE_STABLE, VOLTAGE_STABLE, 1 },
  { 1355000, MAX(VOLTAGE_MIN_START,1325), VOLTAGE_MAX, VOLTAGE_MAX, 0 },
  { 1401000, MAX(VOLTAGE_MIN_START,1325), VOLTAGE_MAX, VOLTAGE_MAX, 1 },
  { 1445000, MAX(VOLTAGE_MIN_START,1325), VOLTAGE_MAX, VOLTAGE_MAX, 1 },
  { 1505000, MAX(VOLTAGE_MIN_START,1325), VOLTAGE_MAX, VOLTAGE_MAX, 0 },
/* EXPERIMENTAL USE AT YOUR OWN RISK */
#ifdef CONFIG_EXPERIMENTAL_FREQ_TABLE
  { 1547000, MAX(VOLTAGE_MIN_START,1350), VOLTAGE_MAX, VOLTAGE_MAX, 1 },
  { 1603000, MAX(VOLTAGE_MIN_START,1350), VOLTAGE_MAX, VOLTAGE_MAX, 0 },
  { 1656000, MAX(VOLTAGE_MIN_START,1350), VOLTAGE_MAX, VOLTAGE_MAX, 1 },
  { 1703000, MAX(VOLTAGE_MIN_START,1350), VOLTAGE_MAX, VOLTAGE_MAX, 1 },
  { 1754000, MAX(VOLTAGE_MIN_START,1350), VOLTAGE_MAX, VOLTAGE_MAX, 0 },
  { 1802000, MAX(VOLTAGE_MIN_START,1375), VOLTAGE_MAX, VOLTAGE_MAX, 1 },
  { 1856000, MAX(VOLTAGE_MIN_START,1375), VOLTAGE_MAX, VOLTAGE_MAX, 0 },
#endif
  { 0 },
};

#if defined(CONFIG_CPU_FREQ_VDD_LEVELS) && defined(CONFIG_MSM_CPU_AVS)
ssize_t acpuclk_get_vdd_levels_str(char *buf) {
  int i, len = 0;
  if (buf) {
    for (i = 0; acpu_vdd_tbl[i].acpu_khz; i++) {
      if (acpu_vdd_tbl[i].ignore==0) {
	len += sprintf(buf + len, "%8u: %4d %4d\n", acpu_vdd_tbl[i].acpu_khz, acpu_vdd_tbl[i].min_vdd, acpu_vdd_tbl[i].max_vdd);
      }
    }
  }
  return len;
}

void acpuclk_set_vdd(unsigned acpu_khz, int min_vdd, int max_vdd    ) {
  int i;
  min_vdd = min_vdd / 25 * 25;	//! regulator only accepts multiples of 25 (mV)
  max_vdd=max_vdd/25*25;

  mutex_lock(&avs_lock);
 
  for (i = 0; acpu_vdd_tbl[i].acpu_khz; i++) {
    if (acpu_khz == 0) {
      acpu_vdd_tbl[i].min_vdd = min(max((acpu_vdd_tbl[i].min_vdd + min_vdd), BRAVO_TPS65023_MIN_UV_MV), BRAVO_TPS65023_MAX_UV_MV);
      acpu_vdd_tbl[i].max_vdd = min(max((acpu_vdd_tbl[i].max_vdd + max_vdd), BRAVO_TPS65023_MIN_UV_MV), BRAVO_TPS65023_MAX_UV_MV);
    } else if (acpu_vdd_tbl[i].acpu_khz == acpu_khz) {
      acpu_vdd_tbl[i].min_vdd = min(max(min_vdd, BRAVO_TPS65023_MIN_UV_MV), BRAVO_TPS65023_MAX_UV_MV);
      acpu_vdd_tbl[i].max_vdd = min(max(max_vdd, BRAVO_TPS65023_MIN_UV_MV), BRAVO_TPS65023_MAX_UV_MV);
    }
  }

  avs_reset_delays(AVSDSCR_INPUT);
  avs_set_tscsr(TSCSR_INPUT);
  
  mutex_unlock(&avs_lock);
}

#endif // CONFIG_CPU_FREQ_VDD_LEVELS


/*
 *  Update the AVS voltage vs frequency table, for current temperature
 *  Adjust based on the AVS delay circuit hardware status
 */
static void avs_update_voltage_table(short *vdd_table)
{
	u32 avscsr;
	int cpu;
	int vu;
	int l2;
	u32 cur_freq_idx, f;
	short cur_voltage;

	cur_freq_idx = avs_state.freq_idx;
	cur_voltage = avs_state.vdd;

	avscsr = avs_test_delays();
	AVSDEBUG("vdd_table=0x%X\n", (unsigned int)vdd_table);
	AVSDEBUG("avscsr=%x, avsdscr=%x, cur_voltage=%d\n", avscsr, avs_get_avsdscr(), cur_voltage);

	/*
	 * Read the results for the various unit's AVS delay circuits
	 * 2=> up, 1=>down, 0=>no-change
	 */
	cpu = ((avscsr >> 23) & 2) + ((avscsr >> 16) & 1);
	vu  = ((avscsr >> 28) & 2) + ((avscsr >> 21) & 1);
	l2  = ((avscsr >> 29) & 2) + ((avscsr >> 22) & 1);
	AVSDEBUG("cpu=%d, vu=%d, l2=%d\n", cpu, vu, l2);

	if ((cpu == 3) || (vu == 3) || (l2 == 3)) {
		printk(KERN_ERR "AVS: Dly Synth O/P error\n");
	} else if ((cpu == 2) || (l2 == 2) || (vu == 2)) {
		/*
		 * even if one oscillator asks for up, increase the voltage,
		 * as its an indication we are running outside the
		 * critical acceptable range of v-f combination.
		 */
		AVSDEBUG("cpu=%d l2=%d vu=%d\n", cpu, l2, vu);
		AVSDEBUG("Voltage up at %d\n", cur_freq_idx);

		cur_voltage += VOLTAGE_STEP;
		if (cur_voltage > MIN(acpu_vdd_tbl[cur_freq_idx].max_vdd_hard_limit, acpu_vdd_tbl[cur_freq_idx].max_vdd)) {
		  printk(KERN_ERR
			 "AVS: %d: Voltage (%d) can not get high enough! Max=%d\n",
			 acpu_vdd_tbl[cur_freq_idx].acpu_khz,
			 cur_voltage,
			 MIN(acpu_vdd_tbl[cur_freq_idx].max_vdd_hard_limit, acpu_vdd_tbl[cur_freq_idx].max_vdd));
		  cur_voltage=MIN(acpu_vdd_tbl[cur_freq_idx].max_vdd_hard_limit, acpu_vdd_tbl[cur_freq_idx].max_vdd);
	}

		// Raise the voltage only for the current frequency
		//vdd_table[cur_freq_idx]=cur_voltage;
		// Raise the voltage for the current frequency and all frequencies that are above it
		for (f=cur_freq_idx; acpu_vdd_tbl[f].acpu_khz; f++) {
		  vdd_table[f]=MAX(vdd_table[f], cur_voltage);
		}
	} else if ((cpu == 1) && (l2 == 1) && (vu == 1)) {
	  // Only lower vdd for the current frequency
	    AVSDEBUG("Trying to lower %d voltage to %d.\n", acpu_vdd_tbl[cur_freq_idx].acpu_khz, cur_voltage);
	  cur_voltage -= VOLTAGE_STEP;
	  if (cur_voltage < MAX(VOLTAGE_MIN, acpu_vdd_tbl[cur_freq_idx].min_vdd)) {
	    AVSDEBUG("Locked at %d\n", MAX(VOLTAGE_MIN, acpu_vdd_tbl[cur_freq_idx].min_vdd));
	    cur_voltage=MAX(VOLTAGE_MIN, acpu_vdd_tbl[cur_freq_idx].min_vdd);
	  }
	  vdd_table[cur_freq_idx]=cur_voltage;
	}
}

/*
 * Return the voltage for the target performance freq_idx and optionally
 * use AVS hardware to check the present voltage freq_idx
 */
static short avs_get_target_voltage(int freq_idx, bool update_table)
{
	unsigned	cur_tempr = GET_TEMPR();
	unsigned	temp_index = cur_tempr*avs_state.freq_cnt;
	short		*vdd_table;

	/* Table of voltages vs frequencies for this temp */
	vdd_table = avs_state.avs_v + temp_index;

	AVSDEBUG("vdd_table[%d]=%d\n", freq_idx, vdd_table[freq_idx]);
	if ((update_table) || (vdd_table[freq_idx]==VOLTAGE_MAX)) {
	  avs_update_voltage_table(vdd_table);
	}

	if (vdd_table[freq_idx] > acpu_vdd_tbl[freq_idx].max_vdd) {
		pr_info("%dmV too high for %d.\n", vdd_table[freq_idx], acpu_vdd_tbl[freq_idx].acpu_khz);
		vdd_table[freq_idx] = acpu_vdd_tbl[freq_idx].max_vdd;
	}
	if (vdd_table[freq_idx] < acpu_vdd_tbl[freq_idx].min_vdd) {
		pr_info("%dmV too low for %d.\n", vdd_table[freq_idx], acpu_vdd_tbl[freq_idx].acpu_khz);
		vdd_table[freq_idx] = acpu_vdd_tbl[freq_idx].min_vdd;
	}

	return vdd_table[freq_idx];
}


/*
 * Set the voltage for the freq_idx and optionally
 * use AVS hardware to update the voltage
 */
static int avs_set_target_voltage(int freq_idx, bool update_table)
{
	int ctr = 5, rc = 0, new_voltage;

	if (freq_idx < 0 || freq_idx >= avs_state.freq_cnt) {
		AVSDEBUG("Out of range :%d\n", freq_idx);
		return -EINVAL;
	}

	new_voltage = avs_get_target_voltage(freq_idx, update_table);
	if (avs_state.vdd != new_voltage) {
	  /*AVSDEBUG*/
	  pr_info("AVS setting V to %d mV @%d MHz\n", new_voltage, acpu_vdd_tbl[freq_idx].acpu_khz / 1000);
		rc = avs_state.set_vdd(new_voltage);
		while (rc && ctr) {
			rc = avs_state.set_vdd(new_voltage);
			ctr--;
			if (rc) {
				printk(KERN_ERR "avs_set_target_voltage: Unable to set V to %d mV (attempt: %d)\n", new_voltage, 5 - ctr);
				mdelay(1);
			}
		}
		if (rc)
			return rc;
		avs_state.vdd = new_voltage;
	}
	return rc;
}

/*
 * Notify avs of clk frquency transition begin & end
 */
int avs_adjust_freq(u32 freq_idx, int begin)
{
	int rc = 0;

	if (!avs_state.set_vdd) {
		/* AVS not initialized */
		return 0;
	}

	if (freq_idx < 0 || freq_idx >= avs_state.freq_cnt) {
		AVSDEBUG("Out of range :%d\n", freq_idx);
		return -EINVAL;
	}

	mutex_lock(&avs_lock);
	if ((begin && (freq_idx > avs_state.freq_idx)) ||
	    (!begin && (freq_idx < avs_state.freq_idx))) {
		/* Update voltage before increasing frequency &
		 * after decreasing frequency
		 */
		rc = avs_set_target_voltage(freq_idx, 0);
		if (rc)
			goto aaf_out;

		avs_state.freq_idx = freq_idx;
	}
	avs_state.changing = begin;
aaf_out:
	mutex_unlock(&avs_lock);

	return rc;
}


static struct task_struct  *kavs_task;
#define AVS_DELAY ((CONFIG_HZ * 50 + 999) / 1000)

static int do_avs_timer(void *data)
{
	int cur_freq_idx;

	while(1) {
		if (kthread_should_stop())
			break;

		mutex_lock(&avs_lock);
		if (!avs_state.changing) {
			/* Only adjust the voltage if clk is stable */
			cur_freq_idx = avs_state.freq_idx;
			avs_set_target_voltage(cur_freq_idx, 1);
		}
		mutex_unlock(&avs_lock);

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout (AVS_DELAY);
	}

	return 0;
}

static int __init avs_work_init(void)
{
	struct sched_param param = { .sched_priority = MAX_RT_PRIO-1 };
	kavs_task = kthread_create(do_avs_timer, NULL,
				   "avs");

	if (IS_ERR(kavs_task)) {
		printk(KERN_ERR "AVS initialization failed\n");
		return -EFAULT;
	}
	printk(KERN_ERR "AVS initialization success\n");

	sched_setscheduler_nocheck(kavs_task, SCHED_RR, &param);
	get_task_struct(kavs_task);

	return 1;
}

static void __exit avs_work_exit(void)
{
	kthread_stop(kavs_task);
	put_task_struct(kavs_task);
}

int __init avs_init(int (*set_vdd)(int), u32 freq_cnt, u32 freq_idx)
{
	int i;

	mutex_init(&avs_lock);

	if (freq_cnt == 0)
		return -EINVAL;

	avs_state.freq_cnt = freq_cnt;

	if (freq_idx >= avs_state.freq_cnt)
		return -EINVAL;

	avs_state.avs_v = kmalloc(TEMPRS * avs_state.freq_cnt *
		sizeof(avs_state.avs_v[0]), GFP_KERNEL);

	if (avs_state.avs_v == 0)
		return -ENOMEM;

	for (i = 0; i < TEMPRS*avs_state.freq_cnt; i++)
		avs_state.avs_v[i] = VOLTAGE_MAX;

	avs_reset_delays(AVSDSCR_INPUT);
	avs_set_tscsr(TSCSR_INPUT);

	avs_state.set_vdd = set_vdd;
	avs_state.changing = 0;
	avs_state.freq_idx = -1;
	avs_state.vdd = -1;
	avs_adjust_freq(freq_idx, 0);

	avs_work_init();

	return 0;
}

void __exit avs_exit()
{
	avs_work_exit();

	kfree(avs_state.avs_v);
}

/* Dump the current calculated vdd table
 */
ssize_t	avs_get_vdd_table_str(char *buf) {
  char		*ptr=buf; 
  unsigned	tempIndex = GET_TEMPR()*avs_state.freq_cnt;
  unsigned	f;
  ssize_t	len;
  short		*vdd_table;
  
  /* Table of voltages vs frequencies for this temp */
  vdd_table = avs_state.avs_v + tempIndex;
  
  for (f=0; acpu_vdd_tbl[f].acpu_khz; f++) {
    if (acpu_vdd_tbl[f].ignore==0) {
      len=sprintf(ptr, "%8u: %4d\n", acpu_vdd_tbl[f].acpu_khz, vdd_table[f]);
      ptr+=len;
    }
  }
  *ptr=0;
  return (strlen(buf));
}

/* Dump all the temperature voltage tables
 */
ssize_t	avs_get_vdd_tables_str(char *buf) {
  char		*ptr=buf; 
  unsigned	tempIndex = GET_TEMPR();
  unsigned	f;
  ssize_t	len;
  short		*vdd_table;

  // Start by dumping the current temperature index
  len=sprintf(ptr, "TempIdx=%d\n", tempIndex);
  ptr += len;

  // And now dump all the vdds for all temperatures and frequencies
  for (f=0; acpu_vdd_tbl[f].acpu_khz; f++) {
    if (acpu_vdd_tbl[f].ignore==0) {
      len=sprintf(ptr, "%8u:", acpu_vdd_tbl[f].acpu_khz);
      ptr += len;
      
      for (tempIndex=0; tempIndex<TEMPRS; tempIndex++) {
	vdd_table=avs_state.avs_v + (tempIndex*avs_state.freq_cnt);
	len=sprintf(ptr, " %4d", vdd_table[f]);
	ptr+=len;
      }
      *ptr='\n';
      ptr++;
    }
  }
  *ptr=0;
  return (strlen(buf));
}
