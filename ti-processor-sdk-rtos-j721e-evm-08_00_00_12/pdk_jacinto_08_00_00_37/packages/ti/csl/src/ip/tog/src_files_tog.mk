
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 am64x))
PACKAGE_SRCS_COMMON += cslr_tog.h csl_tog.h src/ip/tog/src_files_tog.mk src/ip/tog/V0
SRCDIR += src/ip/tog/V0/priv
INCDIR += . src/ip/tog/V0
SRCS_COMMON += csl_slv_tog.c
endif

ifeq ($(SOC),$(filter $(SOC), j7200 j721s2 am64x))
SRCS_COMMON += csl_mst_tog.c
endif
