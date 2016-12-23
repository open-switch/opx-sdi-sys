LIBRARIES=sdi_sys_v2

sdi_sys_v2_SRCS+=sdi_startup.c sdi_entity.c sdi_entity_framework.c
sdi_sys_v2_SRCS+=sdi_thermal.c
sdi_sys_v2_SRCS+=sdi_led.c sdi_media.c
sdi_sys_v2_SRCS+=sdi_fan.c
sdi_sys_v2_SRCS+=sdi_entity_info.c
sdi_sys_v2_SRCS+=sdi_entity_reset.c
sdi_sys_v2_SRCS+=sdi_nvram.c

sdi_sys_v2_LDFLAGS= -lsdi_framework -levent_log -ldn_common -z defs
sdi_sys_v2_CFLAGS= -Wall -Werror

UNIT_TEST+= sdi_ut
sdi_ut_SRCS= unittest/sdi_reference.cpp unittest/sdi_target.cpp unittest/sdi_unit_test.cpp
sdi_ut_LDFLAGS= -lsdi_sys_v2 -ldn_common -lsdi_framework
sdi_ut_CFLAGS=-I../../inc/ -Wall -Werror

include ${MAKE_INC}/workspace.mak
