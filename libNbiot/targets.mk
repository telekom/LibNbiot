# Don't change anything in this file, unless you no what you are doing.
.SUFFIXES:


TARGETDIRS := $(addprefix $(BUILDDIR)/,$(PLATFORMS))


.PHONY: OVERALLTARGET clean $(TARGETDIRS)


OVERALLTARGET: $(TARGETDIRS)


define PLATFORMRULE
$(BUILDDIR)/$(P):
	[ -d $$@ ] || mkdir -p $$@
	[ -d $(LIBDIR)/$(P) ] || mkdir -p $(LIBDIR)/$(P)
	+@$(MAKE) -C $$@ -f $(CURDIR)/Makefile \
            SRCDIR=$(CURDIR)/src INCDIR=$(CURDIR)/include \
            ARCH=$(P) LIB=$(CURDIR)/$(LIBDIR)/$(P) \
	    EXTINC=$(CURDIR)/$(EXT)/include \
            COREINC=$(CURDIR)/$(CORE)/include $(MAKECMDGOALS)
endef
$(foreach P,$(PLATFORMS),$(eval $(PLATFORMRULE)))


Makefile : ;
%.mk :: ;


% :: $(TARGETDIRS) ; :


clean:
	rm -rf $(BUILDDIR)
	rm -rf $(LIBDIR)
