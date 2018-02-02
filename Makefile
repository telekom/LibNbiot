#############################################################################
# Makefile for building nbiot libraries
#############################################################################


# Define any target platforms here. Only targets defined here will be build.
# Don't forget to add appropriate cases below.
#
# List of currently supported target platforms:
#
# export PlATFORMS := x86 x86-debug x86_64 x86_64-debug arm-cortx-m3 arm-cortx-m3-debug
export PLATFORMS := x86


# Internal variables
LIBDIR := lib
INCDIR := include

# Targets
.PHONY: LibNbiotCore LibNbiot directories


.NOT_PARALLEL: LibNbiotCore LibNbiot


all: LibNbiotCore LibNbiot directories


LibNbiotCore: directories
	$(MAKE) -j -e -C LibNbiotCore
	cp -r LibNbiotCore/lib/* $(LIBDIR)/.
	cp -r LibNbiotCore/include/* $(INCDIR)/.


LibNbiot: directories
	$(MAKE) -j -e -C LibNbiot
	cp -r LibNbiot/lib/* $(LIBDIR)/.
	cp -r LibNbiot/include/* $(INCDIR)/.


directories:
	@echo $(INCDIR)
	[ -d $(INCDIR) ] || mkdir -p $(INCDIR)
	[ -d $(LIBDIR) ] || mkdir -p $(LIBDIR)


clean:
	$(MAKE) clean -C LibNbiotCore
	$(MAKE) clean -C LibNbiot
	rm -rf $(LIBDIR) $(INCDIR)
