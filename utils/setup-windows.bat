:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: LibNbiot: Makefile for building external libraries
::
:: Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
:: Krautscheid, Kolja Vornholt, T-Systems International GmbH
:: contact: libnbiot@t-systems.com, opensource@telekom.de
::
:: This file is distributed under the conditions of the Apache License,
:: Version 2.0 (the "License");
:: you may not use this file except in compliance with the License.
:: For details see the file LICENSE at the toplevel.
::
:: Unless required by applicable law or agreed to in writing, software
:: distributed under the License is distributed on an "AS IS" BASIS,
:: WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
:: See the License for the specific language governing permissions and
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



:: Simple shell script to copy windows compatible makefiles to
:: their destination folders. The copied files have been tested
:: with MinGW.

xcopy /y Makefile.win.toplevel ..\Makefile
xcopy /y Makefile.win.nbiot ..\libNbiot\Makefile
xcopy /y targets.mk.win.nbiot ..\libNbiot\targets.mk
xcopy /y Makefile.win.nbiotcore ..\libNbiotCore\Makefile
xcopy /y targets.mk.win.nbiotcore ..\libNbiotCore\targets.mk
xcopy /y Makefile.win.external ..\external\Makefile
xcopy /y targets.mk.win.external ..\external\targets.mk
