ACLOCAL_AMFLAGS=-I m4

SUBDIRS = inc

COMMON_HARDEN_FLAGS=-fexceptions -fstack-protector-strong -fstack-protector-all -D_FORTIFY_SOURCE=2 -Wall -Wformat -Wformat-security -Werror
C_HARDEN_FLAGS=-Wimplicit-function-declaration
LD_HARDEN_FLAGS=-Wl,-z,defs -Wl,-z,now


lib_LTLIBRARIES = libopx_sdi_framework.la
libopx_sdi_framework_la_SOURCES = \
        src/framework/sdi_bus_api.c \
        src/framework/sdi_bus_framework.c \
        src/framework/sdi_driver_framework.c \
        src/framework/sdi_i2c_bus_api.c \
        src/framework/sdi_io_port_api.c \
        src/framework/sdi_pin_bus_api.c \
        src/framework/sdi_pin_group_bus_api.c \
        src/framework/sdi_resource_framework.c \
        src/framework/sdi_smartfusion_io_bus_api.c \
        src/framework/sdi_bmc_bus_api.c \
        src/framework/sdi_bmc_db.cpp \
        src/framework/sdi_fpga_pci_bus_api.c

libopx_sdi_framework_la_LIBADD = -ldl -lopx_common -lopx_logging -lpthread
libopx_sdi_framework_la_LDFLAGS = -version-info 1:1:0 -shared $(LD_HARDEN_FLAGS)

libopx_sdi_framework_la_CPPFLAGS = -I$(top_srcdir)/inc/opx/private -I$(top_srcdir)/inc/opx  -I$(includedir)/opx -fpic $(COMMON_HARDEN_FLAGS)
libopx_sdi_framework_la_CFLAGS = $(C_HARDEN_FLAGS)


#The sdi-sys library
lib_LTLIBRARIES += libopx_sdi_sys.la

libopx_sdi_sys_la_SOURCES = \
        src/hwcore/sdi_entity.c \
        src/hwcore/sdi_entity_reset.c \
        src/hwcore/sdi_comm_dev.c \
        src/hwcore/sdi_nvram.c \
        src/hwcore/sdi_startup.c \
        src/hwcore/sdi_entity_info.c \
        src/hwcore/sdi_entity_framework.c \
        src/hwcore/sdi_thermal.c \
        src/hwcore/sdi_fan.c \
        src/hwcore/sdi_host_system.c \
        src/hwcore/sdi_media.c \
        src/hwcore/sdi_power_monitor.c \
        src/hwcore/sdi_led.c \
        src/hwcore/sdi_ext_ctrl.c

libopx_sdi_sys_la_LIBADD = libopx_sdi_framework.la -lopx_common -lopx_logging
libopx_sdi_sys_la_LDFLAGS = -version-info 1:1:0 -shared $(LD_HARDEN_FLAGS)
libopx_sdi_sys_la_CFLAGS = $(C_HARDEN_FLAGS)
libopx_sdi_sys_la_CPPFLAGS = -I$(top_srcdir)/inc/opx  -I$(includedir)/opx  -I$(top_srcdir)/inc/opx/private -fpic $(COMMON_HARDEN_FLAGS)


lib_LTLIBRARIES += libopx_sdi_device_drivers.la
libopx_sdi_device_drivers_la_SOURCES = \
        src/drivers/sdi_comm_dev_driver.c \
        src/drivers/sdi_cpld.c \
        src/drivers/sdi_cpld_driver.c \
        src/drivers/sdi_cpld_pin.c \
        src/drivers/sdi_cpld_pin_group.c \
        src/drivers/sdi_dell_eeprom.c \
        src/drivers/sdi_eeprom.c \
        src/drivers/sdi_emc142x.c \
        src/drivers/sdi_emc2305.c \
        src/drivers/sdi_entity_info_vdriver.c \
        src/drivers/sdi_i2cmux_pca.c \
        src/drivers/sdi_i2cmux_pin.c \
        src/drivers/sdi_ina219.c \
        src/drivers/sdi_io_bus.c \
        src/drivers/sdi_max6620.c \
        src/drivers/sdi_max6699.c \
        src/drivers/sdi_media_phy_mgmt.c \
        src/drivers/sdi_mono_color_pin_led.c \
        src/drivers/sdi_nvram.c \
        src/drivers/sdi_onie_eeprom.c \
        src/drivers/sdi_pmbus_dev.c \
        src/drivers/sdi_pseudo_bus.c \
        src/drivers/sdi_qsfp_4X1_1000baseT.c \
        src/drivers/sdi_qsfp.c \
        src/drivers/sdi_qsfp28_dd.c \
        src/drivers/sdi_qsfp28_dd_eeprom.c \
        src/drivers/sdi_qsfp_eeprom.c \
        src/drivers/sdi_s6k_psu.c \
        src/drivers/sdi_segment_display.c \
        src/drivers/sdi_seven_segment_pin_led.c \
        src/drivers/sdi_sf_entity_info.c \
        src/drivers/sdi_sf_fan.c \
        src/drivers/sdi_sf_io_bus.c \
        src/drivers/sdi_sfp.c \
        src/drivers/sdi_sfp_eeprom.c \
        src/drivers/sdi_sf_tmp.c \
        src/drivers/sdi_tmp75.c \
        src/drivers/sdi_fixed_port.c \
        src/drivers/sys-interface-drivers/sdi_gpio.c \
        src/drivers/sys-interface-drivers/sdi_i2cdev.c\
        src/drivers/sys-interface-drivers/sdi_sysfs_gpio_helpers.c \
        src/drivers/sdi_bmc.c \
        src/drivers/sdi_bmc_dev.c \
        src/drivers/sdi_bmc_entity_info.c \
        src/drivers/sdi_bmc_fan.c \
        src/drivers/sdi_bmc_io_bus.c \
        src/drivers/sdi_bmc_tmp.c \
        src/drivers/sdi_fpga_pci_bus.c \
        src/drivers/sdi_comm_dev_ext_ctrl.c \
        src/drivers/sdi_platform_util.c \
        src/drivers/sdi_sf_ext_ctrl.c


libopx_sdi_device_drivers_la_LIBADD = libopx_sdi_framework.la -lopx_common -lopx_logging -lz -lm -lpthread -lOpenIPMI -lOpenIPMIposix -lOpenIPMIpthread
libopx_sdi_device_drivers_la_LDFLAGS = -version-info 1:1:0 -shared $(LD_HARDEN_FLAGS)
libopx_sdi_device_drivers_la_CFLAGS = -I$(top_srcdir)/inc/opx -I$(top_srcdir)/inc/opx/private -I$(top_srcdir)/inc/opx/private/eeprom -I$(includedir)/opx -fpic $(COMMON_HARDEN_FLAGS) $(C_HARDEN_FLAGS)

data_sdidir= $(sysconfdir)/opx/sdi
data_sdi_DATA = cfg/sdi/*.sql

lib_LTLIBRARIES += libopx_sdi_db.la

libopx_sdi_db_la_SOURCES = src/vmdb/sdi_db_ops.c
libopx_sdi_db_la_CPPFLAGS = -I$(top_srcdir)/inc/opx -I$(top_srcdir)/inc/opx/private -I$(includedir)/opx -fpic $(COMMON_HARDEN_FLAGS) $(C_HARDEN_FLAGS)
libopx_sdi_db_la_LDFLAGS = -version-info 1:1:0 -shared $(LD_HARDEN_FLAGS)
libopx_sdi_db_la_LIBADD = -lopx_db_sql

lib_LTLIBRARIES += libopx_sdi_sys_vm.la
libopx_sdi_sys_vm_la_SOURCES = \
        src/vmcore/sdi_vm_comm_dev.c \
        src/vmcore/sdi_vm_entity.c \
        src/vmcore/sdi_vm_fan.c \
        src/vmcore/sdi_vm_host_system.c \
        src/vmcore/sdi_vm_power_monitor.c \
        src/vmcore/sdi_vm_led.c \
        src/vmcore/sdi_vm_media.c \
        src/vmcore/sdi_vm_nvram.c \
        src/vmcore/sdi_vm_sys.c \
        src/vmcore/sdi_vm_thermal.c \
        src/vmcore/sdi_vm_ext_ctrl.c

libopx_sdi_sys_vm_la_CPPFLAGS = -I$(top_srcdir)/inc/opx  -I$(top_srcdir)/inc/opx/private -I$(includedir)/opx -fpic $(COMMON_HARDEN_FLAGS) $(C_HARDEN_FLAGS)
libopx_sdi_sys_vm_la_LDFLAGS = -version-info 1:1:0 -shared $(LD_HARDEN_FLAGS)
libopx_sdi_sys_vm_la_LIBADD = libopx_sdi_db.la -lopx_logging -lpthread
