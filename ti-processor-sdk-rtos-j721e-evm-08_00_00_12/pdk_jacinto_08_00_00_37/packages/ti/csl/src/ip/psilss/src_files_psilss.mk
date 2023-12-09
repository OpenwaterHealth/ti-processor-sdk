
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 am64x))
PACKAGE_SRCS_COMMON += cslr_psilss.h src/ip/psilss/src_files_psilss.mk src/ip/psilss/V0
INCDIR += src/ip/psilss/V0
endif
