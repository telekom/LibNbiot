#############################################################################
# LibNbiot: targets.mk
#
# Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
# Krautscheid, Kolja Vornholt, T-Systems International GmbH
# contact: libnbiot@t-systems.com, opensource@telekom.de
#
# This file is distributed under the conditions of the Apache License,
# Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# For details see the file LICENSE at the toplevel.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#############################################################################

# Don't change anything in this file, unless you know what you are doing.
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
	    EXTLIB=$(CURDIR)/$(EXT)/lib/$(P) \
            COREINC=$(CURDIR)/$(CORE)/include $(MAKECMDGOALS)
endef
$(foreach P,$(PLATFORMS),$(eval $(PLATFORMRULE)))


Makefile : ;
%.mk :: ;


% :: $(TARGETDIRS) ; :


clean:
	rm -rf $(BUILDDIR)
	rm -rf $(LIBDIR)
