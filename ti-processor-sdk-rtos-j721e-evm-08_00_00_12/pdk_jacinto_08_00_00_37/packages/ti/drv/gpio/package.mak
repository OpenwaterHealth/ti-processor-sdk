#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = ti/drv/gpio
XDCINCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(XPKGPATH))))
XDCCFGDIR = package/cfg/

#
# The following dependencies ensure package.mak is rebuilt
# in the event that some included BOM script changes.
#
ifneq (clean,$(MAKECMDGOALS))
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/utils.js:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/utils.js
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/xdc.tci:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/xdc.tci
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/template.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/template.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/om2.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/om2.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/xmlgen.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/xmlgen.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/xmlgen2.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/xmlgen2.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/Warnings.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/Warnings.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/IPackage.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/IPackage.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/package.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/package.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/services/global/Clock.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/services/global/Clock.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/services/global/Trace.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/services/global/Trace.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/bld.js:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/bld.js
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/BuildEnvironment.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/BuildEnvironment.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/PackageContents.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/PackageContents.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/_gen.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/_gen.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Library.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Library.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Executable.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Executable.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Repository.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Repository.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Configuration.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Configuration.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Script.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Script.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Manifest.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Manifest.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Utils.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/Utils.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITarget.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITarget.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITarget2.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITarget2.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITarget3.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITarget3.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITargetFilter.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/ITargetFilter.xs
/home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/package.xs:
package.mak: /home/gtbldadm/nightlybuilds/repo_manifests/scripts/jenkins/coresdk_rtos_jacinto_08_00_00_37/xdctools_3_61_04_40_core/packages/xdc/bld/package.xs
package.mak: config_mk.bld
package.mak: package.bld
package.mak: Settings.xdc.xdt
package.mak: GPIOver.h.xdt
endif


all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_ti.drv.gpio.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package ti.drv.gpio" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

ifeq (,$(MK_NOGENDEPS))
-include package/package.cfg.dep
endif

package/package.cfg.xdc.inc: .interfaces $(XDCROOT)/packages/xdc/cfg/cfginc.js package.xdc
	@$(MSG) generating schema include file list ...
	$(CONFIG) -f $(XDCROOT)/packages/xdc/cfg/cfginc.js ti.drv.gpio $@

test:;
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg $(XDCROOT)/packages/xdc/cfg/Main.xs | .interfaces
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,gpio_1_0_0_16
ifeq (,$(MK_NOGENDEPS))
-include package/rel/gpio_1_0_0_16.tar.dep
endif
package/rel/gpio_1_0_0_16/ti/drv/gpio/package/package.rel.xml: package/package.bld.xml
package/rel/gpio_1_0_0_16/ti/drv/gpio/package/package.rel.xml: package/build.cfg
package/rel/gpio_1_0_0_16/ti/drv/gpio/package/package.rel.xml: package/package.xdc.inc
package/rel/gpio_1_0_0_16/ti/drv/gpio/package/package.rel.xml: package/package.cfg.xdc.inc
package/rel/gpio_1_0_0_16/ti/drv/gpio/package/package.rel.xml: .force
	@$(MSG) generating external release references $@ ...
	$(XS) $(JSENV) -f $(XDCROOT)/packages/xdc/bld/rel.js $(MK_RELOPTS) . $@

./packages/gpio_1_0_0_16.tar: package/rel/gpio_1_0_0_16.xdc.inc package/rel/gpio_1_0_0_16/ti/drv/gpio/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/gpio_1_0_0_16.xdc.inc,package/rel/gpio_1_0_0_16.tar.dep)


release release,gpio_1_0_0_16: all ./packages/gpio_1_0_0_16.tar
clean:: .clean
	-$(RM) ./packages/gpio_1_0_0_16.tar
	-$(RM) package/rel/gpio_1_0_0_16.xdc.inc
	-$(RM) package/rel/gpio_1_0_0_16.tar.dep

clean:: .clean
	-$(RM) .libraries $(wildcard .libraries,*)
clean:: 
	-$(RM) .dlls $(wildcard .dlls,*)
#
# The following clean rule removes user specified
# generated files or directories.
#

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard package))
    $(shell $(MKDIR) package)
endif
ifeq (,$(wildcard package/cfg))
    $(shell $(MKDIR) package/cfg)
endif
ifeq (,$(wildcard package/lib))
    $(shell $(MKDIR) package/lib)
endif
ifeq (,$(wildcard package/rel))
    $(shell $(MKDIR) package/rel)
endif
ifeq (,$(wildcard package/internal))
    $(shell $(MKDIR) package/internal)
endif
ifeq (,$(wildcard packages))
    $(shell $(MKDIR) packages)
endif
endif
clean::
	-$(RMDIR) package


