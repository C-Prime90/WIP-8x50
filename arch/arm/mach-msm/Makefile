ifdef_any_of = $(filter-out undefined,$(foreach v,$(1),$(origin $(v))))

obj-y += io.o memory.o timer.o clock.o
obj-$(CONFIG_CPU_FREQ_MSM) += cpufreq.o
obj-y += socinfo.o

obj-$(CONFIG_MSM_PROC_COMM) += proc_comm.o
ifndef CONFIG_ARCH_MSM8X60
	obj-$(CONFIG_MSM_PROC_COMM) += clock-pcom.o
	obj-$(CONFIG_MSM_PROC_COMM) += vreg.o
	ifdef CONFIG_MSM_PROC_COMM
ifndef CONFIG_ARCH_FSM9XXX
		obj-$(CONFIG_REGULATOR) += footswitch-pcom.o
endif
	endif
endif

obj-y += drv_callback.o
obj-y += htc_board_tags.o

obj-$(CONFIG_ARCH_MSM7X00A) += dma.o irq.o acpuclock-arm11.o
obj-$(CONFIG_ARCH_MSM7X30) += dma.o
obj-$(CONFIG_ARCH_MSM_SCORPION) += arch-init-scorpion.o
obj-$(CONFIG_ARCH_QSD8X50) += dma.o acpuclock-qsd8x50.o
obj-$(CONFIG_MSM_CPU_AVS) += avs.o avs_hw.o

obj-$(CONFIG_MSM_VIC) += irq-vic.o

obj-$(CONFIG_MSM_IOMMU) += iommu.o iommu_dev.o devices-iommu.o
obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
obj-$(CONFIG_SMP) += headsmp.o platsmp.o

ifdef CONFIG_CPU_V7
obj-y += idle-v7.o
else
obj-y += idle.o
endif

obj-$(CONFIG_ARCH_QSD8X50) += sirc.o
obj-$(CONFIG_MSM_FIQ_SUPPORT) += fiq_glue.o
obj-$(CONFIG_MSM_SCM) += scm.o scm-boot.o
obj-$(CONFIG_MSM_SMD) += smd.o smd_debug.o
obj-$(CONFIG_MSM_SMD) += smd_tty.o smd_qmi.o
obj-$(CONFIG_MSM_SMD) += last_radio_log.o

ifndef CONFIG_ARCH_MSM9615
ifndef CONFIG_ARCH_MSM8960
ifndef CONFIG_ARCH_MSM8X60
	obj-$(CONFIG_MSM_SMD) += pmic.o
endif
endif
endif
ifndef CONFIG_ARCH_MSM8960
ifndef CONFIG_ARCH_MSM8X60
ifndef CONFIG_ARCH_APQ8064
	obj-y += nand_partitions.o
endif
endif
endif

obj-$(CONFIG_MSM_DAL) += dal.o

obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_device.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_servers.o
obj-$(CONFIG_MSM_QDSP6) += qdsp6/
ifdef CONFIG_PM
	obj-$(CONFIG_ARCH_QSD8X50) += pm2.o
endif

obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o board-trout-mmc.o devices-msm7x00.o
obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o board-trout-mmc.o board-trout-panel.o devices-msm7x00.o
obj-$(CONFIG_MACH_HALIBUT) += board-halibut.o devices-msm7x00.o
obj-$(CONFIG_ARCH_MSM7X30) += board-msm7x30.o devices-msm7x30.o
obj-$(CONFIG_ARCH_QSD8X50) += board-qsd8x50.o devices-qsd8x50.o
obj-$(CONFIG_ARCH_MSM8X60) += board-msm8x60.o
obj-$(CONFIG_ARCH_MSM8960) += board-msm8960.o devices-msm8960.o

obj-$(CONFIG_ARCH_MSM7X30) += gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_QSD8X50) += gpiomux-8x50.o gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_MSM8X60) += gpiomux-8x60.o gpiomux-v2.o gpiomux.o
ifdef CONFIG_MSM_V2_TLMM
ifndef CONFIG_ARCH_MSM8960
# TODO: TLMM Mapping issues need to be resolved
obj-y	+= gpio-v2.o
endif
else
obj-y	+= gpio.o
endif

obj-y += display/

ifneq ($(call ifdef_any_of,CONFIG_MACH_BRAVO CONFIG_MACH_BRAVOC),)
obj-y += board-bravo.o
obj-y += board-bravo-keypad.o board-bravo-mmc.o
obj-y += board-bravo-rfkill.o htc_wifi_nvs.o htc_awb_cal.o
obj-y += board-bravo-wifi.o board-bravo-audio.o
obj-y += msm_vibrator.o
obj-y += board-bravo-microp.o
obj-y += board-bravo-tpa2018d1.o
obj-y += board-bravo-smb329.o
endif

obj-$(CONFIG_HTC_ACOUSTIC_QSD) += htc_acoustic_qsd.o
obj-$(CONFIG_HTC_PWRSPLY) += htc_power_supply.o
obj-$(CONFIG_HTC_35MM_JACK) += htc_35mm_jack.o
obj-$(CONFIG_MICROP_COMMON) += atmega_microp_common.o
obj-$(CONFIG_ARCH_MSM_FLASHLIGHT) += msm_flashlight.o
