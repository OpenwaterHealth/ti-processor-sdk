
ifeq ($(SOC),$(filter $(SOC), tda3xx dra78x am65xx j7200 j721e j721s2 tpr12 awr294x am64x))
PACKAGE_SRCS_COMMON += cslr_dcc.h csl_dcc.h src/ip/dcc/src_files_dcc.mk src/ip/dcc/V0
SRCDIR += src/ip/dcc/V0/priv
INCDIR += src/ip/dcc/V0
SRCS_COMMON += dcc.c
endif
