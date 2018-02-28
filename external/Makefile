#############################################################################
# Makefile for building: libnbiot.a
#############################################################################


# Define any target platforms here. Only targets defined here will be build.
# Don't forget to add appropriate cases below.
# PLATFORMS := default x86 x86-debug x86-shared x86-debug-shared x86_64 x86_64-debug x86_64-shared x86_64-shared-debug arm-cortx-m3 arm-cortx-m3-debug
PLATFORMS := default


# These directories can be renamed but the variables should not be empty
BUILDDIR  := build
LIBDIR    := lib
INCDIR    := include


# Relative path to core library
CORE      := ../libNbiotCore


# Don't change anything from here....
ifneq ($(notdir $(realpath $(CURDIR)/..)),$(BUILDDIR))
include targets.mk
else

VPATH     := $(SRCDIR) $(INC) $(addprefix $(SRCDIR)/,MQTTSNPacket StateMachine List FP)
# ... to here, unless you know what you are doing.


# Variable definitions.
INCLUDES  := $(addprefix -I ,$(VPATH)) -I $(COREINC) -I $(INC)
AR        := ar cq
ARFLAGS   := 
LINKER    := $(AR)
LFLAGS    := $(ARFLAGS)

STMOBJECTS     := $(subst $(SRCDIR)/StateMachine,,$(subst .cpp,.o,$(notdir $(shell find $(SRCDIR)/StateMachine -name "*.cpp"))))
STMINCLUDES    := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/StateMachine -name "*.h")))
STMTARGET      := libstm.a

LISTINCLUDES   := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/List -name "*.h")))

MQTTSNOBJECTS  := $(subst $(SRCDIR)/MQTTSNPacket,,$(subst .c,.co,$(notdir $(shell find $(SRCDIR)/MQTTSNPacket -name "*.c"))))
MQTTSNINCLUDES := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/MQTTSNPacket -name "*.h")))
MQTTSNINCLUDES += $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/MQTTSNClient -name "*.h"))) 
MQTTSNTARGET   := libmqttsnPaho.a

FPINCLUDES     := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/FP -name "*.h")))


# Add special definitions and compile options for target platforms here. Don't
# forget to activate them by adding an entry to PLATFORMS above:
ifeq ($(ARCH),default)
CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Os
CXXFLAGS := -std=c++11 -Wall -g -O0 -ggdb
endif
ifeq ($(ARCH),x86)
CXX       := g++
CXXFLAGS  := -m32 -std=c++0x -Wall -Os
endif
ifeq ($(ARCH),x86-debug)
CXX       := g++
CXXFLAGS  := -m32 -std=c++0x -Wall -D DEBUG -g -O0 -ggdb
endif
ifeq ($(ARCH),x86-shared)
CXX       := g++
CXXFLAGS  := -m32 -std=c++0x -Wall -Os -fPIC
LINKER    := g++
LFLAGS    := -shared -o
TARGET    := libnbiot.so
endif
ifeq ($(ARCH),x86-debug-shared)
CXX       := g++
CXXFLAGS  := -m32 -std=c++0x -Wall -D DEBUG -g -O0 -fPIC -ggdb
LINKER    := g++
LFLAGS    := -shared -o
TARGET    := libnbiot.so
endif
ifeq ($(ARCH),x86_64)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -Os
endif
ifeq ($(ARCH),x86_64-shared)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -Os -fPIC
LINKER    := g++
LFLAGS    := -shared -o
TARGET    := libnbiot.so
endif
ifeq ($(ARCH),x86_64-shared-debug)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -D DEBUG -g -O0 -fPIC -ggdb
LINKER    := g++
LFLAGS    := -shared -o
TARGET    := libnbiot.so
endif
ifeq ($(ARCH),x86_64-debug)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -D DEBUG -g -O0 -ggdb
endif
ifeq ($(ARCH),arm-cortx-m3)
CXX       := arm-none-eabi-gcc
CXXFLAGS  := -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
  -g -Wall -ffunction-sections -Os -ffat-lto-objects -std=c++0x
endif
ifeq ($(ARCH),arm-cortx-m3-debug)
CXX       := arm-none-eabi-gcc
CXXFLAGS  := -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
  -g -Wall -ffunction-sections -D DEBUG -g -O0 -ffat-lto-objects -std=c++0x -ggdb
endif


# Build targets, usually these don't need to be modified.
first: all


all: $(STMTARGET) $(MQTTSNTARGET) $(FPINCLUDES)


$(FPINCLUDES):
	cp $(SRCDIR)/FP/FP.h $(INC)/.


$(MQTTSNTARGET): $(MQTTSNOBJECTS) $(MQTTSNINCLUDES)
	@echo $(MQTTSNOBJECTS)
	$(LINKER) $(LFLAGS) $(MQTTSNTARGET) $(MQTTSNOBJECTS)
	cp $(MQTTSNTARGET) $(LIB)/.


$(MQTTSNINCLUDES):
	cp -f $(SRCDIR)/MQTTSNPacket/*.h $(INC)/.
	cp -f $(SRCDIR)/MQTTSNClient/*.h $(INC)/.


$(STMTARGET): $(STMOBJECTS) $(STMINCLUDES)
	$(LINKER) $(LFLAGS) $(STMTARGET) $(STMOBJECTS)
	cp $(STMTARGET) $(LIB)/.


$(STMINCLUDES):
	cp -f $(SRCDIR)/StateMachine/*.h $(INC)/.


%.o: %.cpp $(LISTINCLUDES)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@


%.co: %.c
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@


$(LISTINCLUDES):
	cp -f $(SRCDIR)/List/*.h $(INC)/.


endif
