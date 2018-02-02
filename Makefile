#############################################################################
# Makefile for building Telekom nbiot libraries
#############################################################################
#
# Define any target platforms here. Only platforms listed here will be build.
# Each platform entry requires the according entries in the Makefiles of the
# subprojects.
# Platforms listed here will override settings in the subprojects. I.e. all
# platforms can be activated in the subprojects but which are actually build
# is controlled here.
#
# Currently these platforms are supported.
# export PlATFORMS := x86 x86-debug x86_64 x86_64-debug arm-cortx-m3 arm-cortx-m3-debug
export PLATFORMS := x86_64 x86_64-shared


# Internal variables
LIBDIR := lib
INCDIR := include


# Targets
.PHONY: LibNbiotCore LibNbiot directories


.NOT_PARALLEL: LibNbiotCore LibNbiot


all: LibNbiotCore LibNbiot


LibNbiotCore LibNbiot: directories
	@echo "building: $@"
	$(MAKE) -j -e -C $@
	@echo "copy build results"
	cp -r $@/lib/* $(LIBDIR)/.
	cp -r $@/include/* $(INCDIR)/.


directories:
	@echo "creating directories for headers and libs"
	[ -d $(INCDIR) ] || mkdir -p $(INCDIR)
	[ -d $(LIBDIR) ] || mkdir -p $(LIBDIR)


clean:
	$(MAKE) clean -C LibNbiotCore
	$(MAKE) clean -C LibNbiot
	rm -rf $(LIBDIR) $(INCDIR)
