ACLOCAL_AMFLAGS=-I m4

SUBDIRS = inc

#The sdi-sys library
lib_LTLIBRARIES = libopx_sdi_sys.la

libopx_sdi_sys_la_SOURCES = src/sdi_entity.c src/sdi_entity_framework.c \
                              src/sdi_entity_info.c src/sdi_entity_reset.c \
                              src/sdi_fan.c src/sdi_led.c src/sdi_media.c src/sdi_startup.c \
                              src/sdi_thermal.c src/sdi_nvram.c


libopx_sdi_sys_la_LDFLAGS = -lopx_sdi_framework -lopx_common -lopx_logging -lpthread -lrt -version-info 1:1:0
libopx_sdi_sys_la_CFLAGS = -I$(top_srcdir)/inc/opx  -I$(includedir)/opx  -I$(top_srcdir)/inc/opx/private


