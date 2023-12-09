
ifeq ($(SOC),$(filter $(SOC), am65xx j721e j721s2 j7200 am64x))
PACKAGE_SRCS_COMMON += cslr_cp_ace.h src/ip/sa/src_files_sa.mk src/ip/sa/V2/cslr_eip_29t2_ram.h src/ip/sa/V3
endif
ifeq ($(SOC),$(filter $(SOC), am263x))
PACKAGE_SRCS_COMMON += src/ip/sa/src_files_sa.mk src/ip/sa/V4/cslr_eip_29t2_ram.h
endif
