
ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e j721s2 am64x tpr12 awr294x))
PACKAGE_SRCS_COMMON += cslr_cpts.h csl_cpts.h src/ip/cpts/src_files_cpts.mk src/ip/cpts/V0
SRCDIR += src/ip/cpts/V0/priv
INCDIR += src/ip/cpts/V0
SRCS_COMMON += csl_cpts.c
endif
