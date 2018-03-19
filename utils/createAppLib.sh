#!/bin/sh

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