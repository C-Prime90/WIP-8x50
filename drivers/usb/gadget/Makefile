ifdef CONFIG_MICROP_COMMON
  obj-$(CONFIG_USB_MSM_72K)	+= msm72k_udc_htc.o
else
  obj-$(CONFIG_USB_MSM_72K)	+= msm72k_udc.o
endif

obj-$(CONFIG_USB_ANDROID) += android.o
obj-$(CONFIG_USB_ANDROID_ACM) += f_acm.o u_serial.o
obj-$(CONFIG_USB_ANDROID_ADB) += f_adb.o
obj-$(CONFIG_USB_ANDROID_MASS_STORAGE) += f_mass_storage.o
obj-$(CONFIG_USB_ANDROID_MTP) += f_mtp.o
obj-$(CONFIG_USB_ANDROID_RNDIS) += f_rndis.o u_ether.o
obj-$(CONFIG_USB_ANDROID_ACCESSORY) += f_accessory.o

# MSM specific
obj-$(CONFIG_USB_ANDROID_DIAG) += diag.o
