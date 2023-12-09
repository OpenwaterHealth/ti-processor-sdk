
ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e j721s2 am64x))
PACKAGE_SRCS_COMMON += cslr_gtc.h src/ip/gtc/src_files_gtc.mk src/ip/gtc/V0
INCDIR += src/ip/gtc/V0
endif
