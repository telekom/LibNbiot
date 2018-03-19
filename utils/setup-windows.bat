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
