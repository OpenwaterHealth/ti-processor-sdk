
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2))
PACKAGE_SRCS_COMMON += cslr_hts.h src/ip/hts/src_files_hts.mk src/ip/hts/V0
INCDIR += src/ip/hts/V0
endif
