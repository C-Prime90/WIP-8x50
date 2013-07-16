ifdef CONFIG_MSM_SOC_REV_A && CONFIG_ARCH_QSD8X50
# QSD8x50A
  zreladdr-y		:= 0x00008000
params_phys-y		:= 0x00000100
initrd_phys-y		:= 0x04000000
elif CONFIG_ARCH_QSD8X50
# QSD8x50
  zreladdr-y		:= 0x20008000
params_phys-y		:= 0x20000100
initrd_phys-y		:= 0x24000000
else
# Default
  zreladdr-y		:= 0x10008000
params_phys-y		:= 0x10000100
initrd_phys-y		:= 0x10800000
endif
