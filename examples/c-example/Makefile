#############################################################################
# LibNbiot: Makefile for building Telekom nbiot libraries
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
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#############################################################################


# Prerequisites: Run "make" in the toplevel directory to build the libraries
# first. The PLATFORM type "default" has to be enabled. See README.md for
# further details.


# Directories
SRCDIR   := src
BUILDDIR := build
BINDIR   := bin


# Compiler and linker settings
CXX      := g++
CXXFLAGS := -std=c++11 -Wall -g -O0 -ggdb
LDFLAGS  := -lnbiot -lstm -lmqttsnpaho -lnbiotcore -pthread -lrt
INCLUDES := ../../include
LIB      := $(CURDIR)/../../lib/default


# Binary target
TARGET   := $(BINDIR)/c-example


# Targets
.PHONY: clean directories


all: $(TARGET)


$(TARGET): directories $(BUILDDIR)/main.co $(BUILDDIR)/serialcom.o $(BUILDDIR)/serial.o
	$(CXX) $(BUILDDIR)/main.co $(BUILDDIR)/serialcom.o $(BUILDDIR)/serial.o -L $(LIB) $(LDFLAGS) -o $(TARGET)


$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) -I $(INCLUDES) -I $(SRCDIR) $< -o $@


$(BUILDDIR)/%.co: $(SRCDIR)/%.c
	$(CC) -c $(CXXFLAGS) -I $(INCLUDES) -I $(SRCDIR) $< -o $@


directories:
	mkdir -p build
	mkdir -p bin

clean:
	rm -rf build
	rm -rf bin
