#!/bin/sh

#############################################################################
# LibNbiot: createAppLib.sh
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


PLATFORM=default

cd ../lib/$PLATFORM

ar -x libmqttsnPaho.a
ar -x libnbiot.a
ar -x libstm.a

ar -r libnbiotApp.a *.o
ar -r libnbiotApp.a *.co

rm -f *.o
rm -f *.co

## optional: ##
rm -f libmqttsnPaho.a
rm -f libnbiot.a
rm -f libstm.a

cd ../../utils