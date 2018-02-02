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


# Targets
.PHONY: LibNbiotCore LibNbiot


.NOT_PARALLEL: LibNbiotCore LibNbiot


all: LibNbiotCore LibNbiot


LibNbiotCore:
	$(MAKE) -j -e -C LibNbiotCore


LibNbiot:
	$(MAKE) -j -e -C LibNbiot 


clean:
	$(MAKE) clean -C LibNbiotCore
	$(MAKE) clean -C LibNbiot
