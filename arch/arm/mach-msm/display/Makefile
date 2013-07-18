ifdef_any_of = $(filter-out undefined,$(foreach v,$(1),$(origin $(v))))

ifneq ($(call ifdef_any_of,CONFIG_MACH_BRAVO CONFIG_MACH_BRAVOC),)
obj-y += board-bravo-panel.o
endif
