After cloning this project, the submodules (libNbiot libNbiotCore) need to be enabled.
This ist performed by executing the commands:

`git submodule init`
`git submodule update`

As the build structure of the submodules is different from the one in 'master' branch, the used
branches have to be changed to 'make':

`cd libNbiot`
`git checkout make`
`cd ..`
`cd libNbiotCore`
`git checkout make`

Afterwards, the project can be compiled by issuing

`make`

in this directory. This will build all libraries for the specified platforms. To activate/deactivate a
platform just add/remove it to/from the

`PLATFORM`

variable in the Makefile. For example, if you only wannt to build for x86_64 the platform variable should
look like this:

`PLATFORM := x86_64`

The Makefiles contains a platform named **default**, which uses the current systems build variables for
compiling.



