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
# export PLATFORMS := default x86 x86-debug x86-shared x86-debug-shared x86_64 x86_64-debug x86_64-shared x86_64-shared-debug arm-cortx-m3 arm-cortx-m3-debug
export PLATFORMS := default 


# Internal variables
LIBDIR   := lib
INCDIR   := include


# Name of lib directories
APPLIB   := libNbiot
CORELIB  := libNbiotCore
EXTERNAL := external


# Targets
.PHONY: $(EXTERNAL) $(CORELIB) $(APPLIB) directories


.NOT_PARALLEL: $(EXTERNAL) $(CORELIB) $(APPLIB)


all: $(EXTERNAL) $(CORELIB) $(APPLIB)


$(APPLIB): $(CORELIB)
	@echo "building: $@"
	$(MAKE) -j -e -C $@ CORE=../$(CORELIB) \
          EXT=../$(EXTERNAL)
	@echo "copy build results"
	cp -r $@/lib/* $(LIBDIR)/.
	cp -r $@/include/* $(INCDIR)/.


$(CORELIB): $(EXTERNAL)
	@echo "building: $@"
	$(MAKE) -j -e -C $@
	@echo "copy build results"
	cp -r $@/lib/* $(LIBDIR)/.
	cp -r $@/include/* $(INCDIR)/.


$(EXTERNAL): directories
	@echo "building: $@"
	$(MAKE) -j -e -C $@ CORE=../$(CORELIB)
	@echo "copy build results"
	cp -r $@/lib/* $(LIBDIR)/.
	cp -r $@/include/* $(INCDIR)/.


directories:
	@echo "creating directories for headers and libs"
	[ -d $(INCDIR) ] || mkdir -p $(INCDIR)
	[ -d $(LIBDIR) ] || mkdir -p $(LIBDIR)


clean:
	$(MAKE) clean -C $(EXTERNAL)
	$(MAKE) clean -C $(CORELIB)
	$(MAKE) clean -C $(APPLIB)
	rm -rf $(LIBDIR) $(INCDIR)
