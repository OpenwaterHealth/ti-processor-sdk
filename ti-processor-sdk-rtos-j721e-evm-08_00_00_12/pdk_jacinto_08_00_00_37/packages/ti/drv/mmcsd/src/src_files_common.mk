
# Common source files across all platforms and cores
SRCS_COMMON += MMCSD_drv.c

PACKAGE_SRCS_COMMON = makefile MMCSD.h mmcsd_component.mk src/MMCSD_osal.h \
                      docs/ReleaseNotes_MMCSD_LLD.pdf \
                      src/MMCSD_drv.c src/MMCSD_drv_log.h \
                      src/v1/MMCSD_v1_lld.h \
                      soc/MMCSD_soc.h \
                      build  src/src_files_common.mk src/Module.xs config_mk.bld MMCSDver.h MMCSDver.h.xdt \
                      package.bld package.xdc package.xs Settings.xdc.xdt

ifeq ($(SOC), $(filter $(SOC), omapl137 omapl138))
SRCDIR = . src src/v0
INCDIR = . src src/v0
SRCS_COMMON += MMCSD_v0.c
PACKAGE_SRCS_COMMON += src/v0 soc/MMCSD_v0.h
else
ifeq ($(SOC), $(filter $(SOC), am65xx j721e j7200 am64x))
SRCDIR = . src src/v2
INCDIR = . src src/v2
SRCS_COMMON += MMCSD_v2.c
PACKAGE_SRCS_COMMON += src/v2 soc/MMCSD_v2.h
else
SRCDIR = . src src/v1
INCDIR = . src src/v1
SRCS_COMMON += MMCSD_v1.c MMCSD_v1_lld.c
PACKAGE_SRCS_COMMON += src/v1 soc/MMCSD_v1.h
endif
endif
