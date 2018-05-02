#############################################################################
# LibNbiot: Makefile for building: libnbiotcore.a
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
# PLATFORMS := default x86 x86-debug x86-shared x86-debug-shared x86_64 x86_64-debug x86_64-shared x86_64-shared-debug arm-cortex-m3 arm-cortex-m3-debug
PLATFORMS := default


# Build and target lib directory (relative paths)
BUILDDIR  := build
LIBDIR    := lib
EXTERNAL  := ../external


# Don't change anything from here....
ifneq ($(notdir $(realpath $(CURDIR)/..)),build)
include targets.mk
else

VPATH     := $(SRCDIR) $(INCDIR) $(EXTINC)
# ... to here, unless you know what you are doing.


# Variable definitions.
INCLUDES  := $(addprefix -I ,$(VPATH))
OBJECTS   := $(subst $(SRCDIR)/,,$(subst .cpp,.o,$(shell find $(SRCDIR) -name "*.cpp")))
AR        := ar
ARFLAGS   := cq
TARGET    := libnbiotcore.a
LINKER    := $(AR)
LFLAGS    := $(ARFLAGS)


# Add special definitions and compile options for target platforms here, don't
# forget to activate them by adding an entry to PLATFORMS above:
ifeq ($(ARCH),default)
CXX      := g++
CXXFLAGS := -std=c++11 -Wall -g -O0 -ggdb -D DEBUG
endif
ifeq ($(ARCH),x86)
CXX      := g++
CXXFLAGS := -m32 -std=c++0x -Wall -Os
endif
ifeq ($(ARCH),x86-debug)
CXX      := g++
CXXFLAGS := -m32 -std=c++0x -Wall -D DEBUG -g -O0 -ggdb
endif
ifeq ($(ARCH),x86-shared)
CXX      := g++
CXXFLAGS := -m32 -std=c++0x -Wall -Os -fPIC
LINKER   := g++
LFLAGS   := -shared -m32 -o
TARGET   := libnbiotcore.so
endif
ifeq ($(ARCH),x86-debug-shared)
CXX      := g++
CXXFLAGS := -m32 -std=c++0x -Wall -D DEBUG -g -O0 -ggdb
LINKER   := g++
LFLAGS   := -shared -m32 -o
TARGET   := libnbiotcore.so
endif
ifeq ($(ARCH),x86_64)
CXX      := g++
CXXFLAGS := -m64 -std=c++0x -Wall -Os
endif
ifeq ($(ARCH),x86_64-debug)
CXX      := g++
CXXFLAGS := -m64 -std=c++0x -Wall -D DEBUG -g -O0 -ggdb
endif
ifeq ($(ARCH),x86_64-shared)
CXX      := g++
CXXFLAGS := -m64 -std=c++0x -Wall -O3 -fPIC
LINKER   := g++
LFLAGS   := -shared -m64 -o
TARGET   := libnbiotcore.so
endif
ifeq ($(ARCH),x86_64-shared-debug)
CXX      := g++
CXXFLAGS := -m64 -std=c++0x -Wall -D DEBUG -g  -O0 -ggdb -fPIC
LINKER   := g++
LFLAGS   := -shared -m64 -o
TARGET   := libnbiotcore.so
endif
ifeq ($(ARCH),arm-cortex-m3)
CXX      := arm-none-eabi-gcc
CXXFLAGS := -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
  -g -Wall -ffunction-sections -Os -ffat-lto-objects -std=c++0x
endif
ifeq ($(ARCH),arm-cortex-m3-debug)
CXX      := arm-none-eabi-gcc
CXXFLAGS := -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
  -g -Wall -ffunction-sections -D DEBUG -g -Og -ffat-lto-objects -std=c++0x
endif


# Build targets, usually these don't need to be modified.
first: all


all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(LINKER) $(LFLAGS) $(TARGET) $(OBJECTS)
	cp -f $(TARGET) $(LIB)/.


%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@


endif
