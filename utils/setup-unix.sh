#!/bin/sh

#############################################################################
# LibNbiot: setup-unix.sh
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
#############################################################################

#
# Simple shell script to copy unix compatible makefiles to
# their destination folders. The copied files have been tested
# with various toolchains.

cp Makefile.unix.toplevel ../Makefile
cp Makefile.unix.nbiot ../libNbiot/Makefile
cp targets.mk.unix.nbiot ../libNbiot/targets.mk
cp Makefile.unix.nbiotcore ../libNbiotCore/Makefile
cp targets.mk.unix.nbiotcore ../libNbiotCore/targets.mk
cp Makefile.unix.external ../external/Makefile
cp targets.mk.unix.external ../external/targets.mk
