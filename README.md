### Install

After cloning this project, the submodules (libNbiot libNbiotCore) need to be enabled.
This is performed by executing the commands:

`git submodule init`
`git submodule update`

### Compile

#### CMake

Create a new directory for CMakeFiles, e.g. build, and change to the directory. Afterwards, you can use cmake and make:

    mkdir build && cd build
    cmake ..
    make

After building the project, the static libraries libnbiot.a and libnbiotcore.a are located in lib/

#### Make

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


### License

Copyright (C) 2018, Edgar Hindemith, Yassine Amraue, Thorsten Krautscheid, Kolja Vornholt, T-Systems International GmbH

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
