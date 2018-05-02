#############################################################################
# LibNbiot: Makefile for building external libraries
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

# Define any target platforms here. Only targets defined here will be build.
# Don't forget to add appropriate cases below.
# PLATFORMS := default x86 x86-debug x86-shared x86-debug-shared x86_64 x86_64-debug x86_64-shared x86_64-shared-debug arm-cortex-m3 arm-cortex-m3-debug msp432-debug
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

VPATH     := $(SRCDIR) $(INC) $(addprefix $(SRCDIR)/,paho.mqtt-sn.embedded-c/MQTTSNPacket StateMachine List FP)
# ... to here, unless you know what you are doing.


# Variable definitions.
INCLUDES  := $(addprefix -I ,$(VPATH)) -I $(COREINC) -I $(INC)
AR        := ar 
ARFLAGS   := cq
LINKER    := $(AR)
LFLAGS    := $(ARFLAGS)

STMOBJECTS     := $(subst $(SRCDIR)/StateMachine,,$(subst .cpp,.o,$(notdir $(shell find $(SRCDIR)/StateMachine -name "*.cpp"))))
STMINCLUDES    := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/StateMachine -name "*.h")))
STMTARGET      := libstm.a

LISTINCLUDES   := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/List -name "*.h")))

MQTTSNOBJECTS  := $(subst $(SRCDIR)/paho.mqtt-sn.embedded-c/MQTTSNPacket,,$(subst .c,.co,$(notdir $(shell find $(SRCDIR)/paho.mqtt-sn.embedded-c/MQTTSNPacket -name "*.c"))))
MQTTSNINCLUDES := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/paho.mqtt-sn.embedded-c/MQTTSNPacket -name "*.h")))
MQTTSNINCLUDES += $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/paho.mqtt-sn.embedded-c/MQTTSNClient -name "*.h"))) 
MQTTSNTARGET   := libmqttsnpaho.a

FPINCLUDES     := $(addprefix $(INC)/,$(notdir $(shell find $(SRCDIR)/FP -name "*.h")))


# Add special definitions and compile options for target platforms here. Don't
# forget to activate them by adding an entry to PLATFORMS above:
ifeq ($(ARCH),default)
CXX      := g++
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
LFLAGS    := -shared -m32 -o
STMTARGET := libstm.so
MQTTSNTARGET := libmqttsnpaho.so
endif
ifeq ($(ARCH),x86-debug-shared)
CXX       := g++
CXXFLAGS  := -m32 -std=c++0x -Wall -D DEBUG -g -O0 -fPIC -ggdb
LINKER    := g++
LFLAGS    := -shared -m32 -o
STMTARGET := libstm.so
MQTTSNTARGET := libmqttsnpaho.so
endif
ifeq ($(ARCH),x86_64)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -Os
endif
ifeq ($(ARCH),x86_64-shared)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -Os -fPIC
LINKER    := g++
LFLAGS    := -shared -m64 -o
STMTARGET := libstm.so
MQTTSNTARGET := libmqttsnpaho.so
endif
ifeq ($(ARCH),x86_64-shared-debug)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -D DEBUG -g -O0 -fPIC -ggdb
LINKER    := g++
LFLAGS    := -shared -m64 -o
STMTARGET := libstm.so
MQTTSNTARGET := libmqttsnpaho.o
endif
ifeq ($(ARCH),x86_64-debug)
CXX       := g++
CXXFLAGS  := -m64 -std=c++0x -Wall -D DEBUG -g -O0 -ggdb
endif
ifeq ($(ARCH),arm-cortex-m3)
CXX       := arm-none-eabi-gcc
CXXFLAGS  := -I /usr/lib/arm-none-eabi/include -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
  -g -Wall -ffunction-sections -Os -ffat-lto-objects -std=c++0x
endif
ifeq ($(ARCH),arm-cortex-m3-debug)
CXX       := arm-none-eabi-gcc
CXXFLAGS  := -I /usr/lib/arm-none-eabi/include -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
  -g -Wall -ffunction-sections -D DEBUG -g -O0 -ffat-lto-objects -std=c++0x -ggdb
endif


# Build targets, usually these don't need to be modified.
.NOTPARALLEL: $(LISTINCLUDES) $(MQTTSNINCLUDES)


first: all


all: $(STMTARGET) $(MQTTSNTARGET) $(FPINCLUDES)


$(FPINCLUDES):
	cp $(SRCDIR)/FP/FP.h $(INC)/.


$(MQTTSNTARGET): $(MQTTSNOBJECTS) $(MQTTSNINCLUDES)
	@echo $(MQTTSNOBJECTS)
	$(LINKER) $(LFLAGS) $(MQTTSNTARGET) $(MQTTSNOBJECTS)
	cp $(MQTTSNTARGET) $(LIB)/.


$(MQTTSNINCLUDES):
	cp -f $(SRCDIR)/paho.mqtt-sn.embedded-c/MQTTSNPacket/*.h $(INC)/.
	cp -f $(SRCDIR)/paho.mqtt-sn.embedded-c/MQTTSNClient/*.h $(INC)/.


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
