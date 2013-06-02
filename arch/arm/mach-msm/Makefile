obj-y += io.o timer.o clock.o socinfo.o
obj-y += htc_board_tags.o drv_callback.o
obj-y += pmic.o gpio.o nand_partitions.o
obj-y += display/

## Wont compile yet
# YAFFS2 driver
obj-y += #pm.o
obj-y += #memory.o

ifeq ($(CPU_V7),)
obj-y += idle-v7.o
else
obj-y += idle.o
endif

obj-$(CONFIG_DEBUG_FS) += clock-debug.o

obj-$(CONFIG_MSM_VIC) += irq-vic.o
obj-$(CONFIG_MSM_IOMMU) += devices-iommu.o

obj-$(CONFIG_ARCH_MSM7X00A) += dma.o irq.o acpuclock-arm11.o
obj-$(CONFIG_ARCH_MSM7X30) += dma.o
obj-$(CONFIG_ARCH_QSD8X50) += dma.o sirc.o

obj-$(CONFIG_MSM_PROC_COMM) += proc_comm.o clock-pcom.o vreg.o

obj-$(CONFIG_MSM_SMD) += smd.o smd_debug.o
obj-$(CONFIG_MSM_SMD) += smd_tty.o smd_qmi.o
obj-$(CONFIG_MSM_SMD) += last_radio_log.o
obj-$(CONFIG_MSM_SCM) += scm.o scm-boot.o

CFLAGS_scm.o :=$(call as-instr,.arch_extension sec,-DREQUIRES_SEC=1)

obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
obj-$(CONFIG_SMP) += headsmp.o platsmp.o

obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o board-trout-mmc.o devices-msm7x00.o
obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o board-trout-mmc.o board-trout-panel.o devices-msm7x00.o
obj-$(CONFIG_MACH_HALIBUT) += board-halibut.o devices-msm7x00.o
obj-$(CONFIG_ARCH_MSM7X30) += board-msm7x30.o devices-msm7x30.o
obj-$(CONFIG_ARCH_QSD8X50) += board-qsd8x50.o devices-qsd8x50.o
obj-$(CONFIG_ARCH_QSD8X50) += devices-qsd8x50.o
obj-$(CONFIG_ARCH_MSM8X60) += board-msm8x60.o
obj-$(CONFIG_ARCH_MSM8960) += board-msm8960.o devices-msm8960.o

obj-$(CONFIG_ARCH_MSM7X30) += gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_QSD8X50) += gpiomux-8x50.o gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_MSM8X60) += gpiomux-8x60.o mgpiomux-v2.o gpiomux.o

obj-$(CONFIG_MACH_BRAVO) += board-bravo.o
obj-$(CONFIG_MACH_BRAVO) += board-bravo-keypad.o board-bravo-mmc.o
obj-$(CONFIG_MACH_BRAVO) += board-bravo-rfkill.o htc_wifi_nvs.o htc_awb_cal.o
obj-$(CONFIG_MACH_BRAVO) += board-bravo-wifi.o board-bravo-audio.o
obj-$(CONFIG_MACH_BRAVO) += msm_vibrator.o
obj-$(CONFIG_MACH_BRAVO) += board-bravo-microp.o
obj-$(CONFIG_MACH_BRAVO) += board-bravo-tpa2018d1.o
obj-$(CONFIG_MACH_BRAVO) += board-bravo-smb329.o

obj-$(CONFIG_MACH_BRAVOC) += board-bravo.o
obj-$(CONFIG_MACH_BRAVOC) += board-bravo-keypad.o board-bravo-mmc.o
obj-$(CONFIG_MACH_BRAVOC) += board-bravo-rfkill.o htc_wifi_nvs.o htc_awb_cal.o
obj-$(CONFIG_MACH_BRAVOC) += board-bravo-wifi.o board-bravo-audio.o
obj-$(CONFIG_MACH_BRAVOC) += msm_vibrator.o
obj-$(CONFIG_MACH_BRAVOC) += board-bravo-microp.o
obj-$(CONFIG_MACH_BRAVOC) += board-bravo-tpa2018d1.o
obj-$(CONFIG_MACH_BRAVOC) += board-bravo-smb329.o

obj-$(CONFIG_HTC_35MM_JACK) += htc_35mm_jack.o

obj-$(CONFIG_ARCH_MSM_FLASHLIGHT) += msm_flashlight.o

obj-$(CONFIG_MSM_DAL) += dal.o

obj-$(CONFIG_REGULATOR) += footswitch-pcom.o

obj-$(CONFIG_HTC_PWRSPLY) += htc_power_supply.o

obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_device.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_servers.o

obj-$(CONFIG_HTC_ACOUSTIC_QSD) += htc_acoustic_qsd.o

obj-$(CONFIG_MSM_FIQ_SUPPORT) += fiq_glue.o

obj-$(CONFIG_CPU_FREQ_MSM) += cpufreq.o

obj-$(CONFIG_ARCH_MSM_SCORPION) += arch-init-scorpion.o

obj-$(CONFIG_ARCH_QSD8X50) += acpuclock-qsd8x50.o

obj-$(CONFIG_MSM_CPU_AVS) += avs.o avs_hw.o

obj-$(CONFIG_MSM_QDSP6) += qdsp6/
