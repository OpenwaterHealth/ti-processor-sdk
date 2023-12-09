#
# Enet SoC library
#
ifeq ($(RULES_MAKE),)
include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
include $(RULES_MAKE)
endif

MODULE_NAME = enetsoc

ifeq ($(SOC),$(filter $(SOC), am65xx))
  SRCDIR += j7x j7x/am65xx
endif
ifeq ($(SOC),$(filter $(SOC), j721e))
  SRCDIR += j7x j7x/j721e
endif
ifeq ($(SOC),$(filter $(SOC), j7200))
  SRCDIR += j7x j7x/j7200
endif
ifeq ($(SOC),$(filter $(SOC), tpr12))
  SRCDIR += tpr12
endif
ifeq ($(SOC),$(filter $(SOC), awr294x))
  SRCDIR += awr294x
endif

INCDIR = .

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
INCLUDE_INTERNAL_INTERFACES = csl

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += enet_soc.c

# J7x common source files
ifeq ($(SOC),$(filter $(SOC), j721e j7200 am65xx))
    SRCS_COMMON += j7x_soc.c
endif

CFLAGS_LOCAL_COMMON = $(PDK_CFLAGS) $(ENET_CFLAGS)

PACKAGE_SRCS_COMMON = enet_soc.h makefile $(SRCDIR)


# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/ti/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
