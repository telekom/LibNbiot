# LibNbiot - A non-blocking MQTT-SN Library for NB-IoT

The repository contains the source code for the LibNbiot, which allows to access the
**Telekom Cloud of Things** with **NB-IoT** radio modules.

## Building from command line

The library relieas on [GNU Make](https://www.gnu.org/software/make/) as build system. After downloading it can
be build on any Unix by navigating into the toplevel directory and executing

    make

This will envoke make in all subdirectories. Furthermore, the directories `lib` and `include` are created
and the build results are copied into them. As the the build system supports multiple cross platform
builds with a single invocation (see below), the `lib` directory contains subdirectories which indicate the target
platform.

To cleanup up all build results perform:

    make clean

The subdirectories contain dedicated makefiles which can be invoked individually. However, there are
some cross dependencies which have to be met: `libNbiotCore` depends on `external` while `libNbiot` depends
on both.

### Building on Windows

The makefiles supplied within the directory structure should work on Linux and MacOS. Additional versions
for building on Windows can be found in the `utils` directory. To switch to Windows-makefiles, navigate to
the utils folder and execute the `setup-win.bat` script. The script will exchange all makefiles in the directory
tree by windows compatible versions. To switch back to the Unix-compliant versions, the bash script `setup-unix.sh`
can be used (also in the `utils` directory).

**Note:** A toolchain like MinGW or Cygwin needs to be installed to build on Windows. All the required programs
(compiler, linker) need to be accessable by the system path.

### Cross platform builds

The build system has been setup to support cross compilation for multiple platforms. In general
the build can be executed for several platforms at once, given that the given platform dependencies
are already installed on the build system.

Currently the following platforms are supported:

* **default** (build setting are mostly taken from the build system, static library, debug settings enabled)
* **x86** (32-bit, static library, optimized for size)
* **x86-debug** (32-bit, static library, debug settings enabled)
* **x86-shared** (32-bit, shared library, optimized for size)
* **x86-debug-shared** (32-bit, shared library, debug settings enabled)
* **x86_64** (64-bit, static library, optimized for size)
* **x86_64-debug** (64-bit, static library, debug settings enabled)
* **x86_64-shared** (64-bit, shared library, optimized for size)
* **x86_64-shared-debug** (64-bit, shared library, debug settings enabled)
* **arm-cortex-m3** (ARM Cortex M3 bare metal, static library, optimized for size)
* **arm-cortex-m3-debug** (ARM Cortex M3 bare metal, static library, debug settings enabled)

By default the only target platform enabled is **default**. To enable another/additional one, just open
the makefile and add it to the PLATFORMS variable. For example the variable should look like this

    export PLATFORMS := arm-cortex-m3 arm-cortex-m3-debug

if debug and release version are to be build for the ARM Cortx M3 architecture.

**Note:** The platform settings are propagated from the toplevel makefile to the makefiles in the
subdirectories, overruling any platforms configured there. While calling make in the subdirectories
will build the sources there with the platform settings in this subdirectorie's makefile, calling the
the toplevel makefile configures platforms for all subdirectories.

#### Adding further architectures

Obviously, each cross platform build requires certain compiler/linker settings unique to the target
platform. These need to be configured in all makefiles which actually compile the source code. For this
reason, the makefiles contain sections where these platform specific settings are configured. For example,
the section for building the debug version of the library for the ARM Cortex M3 architecture looks like this:

    ifeq ($(ARCH),arm-cortex-m3-debug)
    CXX       := arm-none-eabi-gcc
    CXXFLAGS  := -mcpu=cortex-m3 -mthumb -Wa,-alh=nbiot_time.lst \
      -g -Wall -ffunction-sections -D DEBUG -g -O0 -ffat-lto-objects -std=c++0x -ggdb
    endif

It sets the compiler `(CXX)` and compiler flags `(CXXFLAGS)` to use for the build. These variables have to
be set to values which suit the target enviroment. An important detail is the initial if-statement, where
make checks the architecture. In this statement, `arm-cortex-m3-debug` defines the name of the target
environment. If the very same name is added to the `PLATFORMS` entry at the beginning of the makefile, the
target is activated and the sources are build for this platform as well.

The same statemant has to be added to all makefiles which actually compile and link code. This is necessary
to ensure that the compiled code of the subdirectories is compatible with each other. This means the makefiles
found in

* libNbiot
* libNbiotCore
* external

have to include the same if-statement.

#### Troubleshooting:

**x86 build fails on x86_64 platform**

When compiling for 32-bit platforms on 64-bit platform you may encounter this error (or a similar one):  

    /usr/include/c++/6/cstdlib:41:28: fatal error: bits/c++config.h: No such file or directory`

The reason for this is usually, that the GCC multilib feature is enabled, which indicates that the
32-bit version of the libc is not installed. To fix this error install:

* CentOS/RHEL 5.x:  
`yum -y install glibc-devel.i386 libstdc++-devel.i386`  
* CentOS/RHEL 6.x:  
`yum -y install glibc-devel.i686 glibc-devel ibstdc++-devel.i686`  
* Debian/Ubuntu:  
`sudo apt-get install g++-multilib libc6-dev-i386`  
* SLES/OpenSUSE:  
`zypper in glibc-devel-32bit`  

## Example usage

The library comes with two examples for Unix-systems. More examples for different platforms will be added in the future.
They have been tested on Ubuntu 16.04+ and MacOS 10.13.1.

The examples are located at `examples\c-example` and `cpp-example`. The c-example
illustrates the usage in a plain 'C' environment, while the cpp-example makes use of features unique to 'C++'. 

Both examples illustrate basic use cases for communication with the **Telekom Cloud of Things**
by sending generated temperature values in an infinite loop.

### Building

To build the examples, the library has to be compiled with `PLATFORM` containing the `default` architecture (see:
*Cross platfrom builds*, above). After this navigate to the `examples` directory and execute:

    make

This creates the `c-example\bin` and `cpp-example' directories which contain the example executables.

### Running the example

The example requires three parameters to run:
* -i ***IMSI***, a 15-digit IMSI which is registered for the Telekom Cloud of Things
* -p ***password***, the 8-characters password distributed with IMSI for Telekom Cloud Things access
* -s ***serial node***, path to the serial device node (e.g. /dev/ttyACM0).

The example can then be started from the `examples` folder by executing:

    ./c-example/bin/c-example -i "imsi" -p "pw" -s "path to serial node"

similarly the cpp-example can be executed:

    ./cpp-example/bin/cpp-example -i "imsi" -p "pw" -s "path to serial node"

**Note:** Access to serial device nodes may require special privileges, especially on Linux. You can
either run the program as root, by using the above program with `sudo`, add your user to the group `dialout`
or configure a `udev/hotplug` rule to add the serial device node to your users group. The best solution depends
on the exact environment you are using.

#### Sending downlink operations

The example is prepared to receive downlink operations from your Telekom Cloud of Things account. For this, find
your device in the Telekom Cloud of Things and send a shell-command. The command must have the structure:

    topic:MyCmd;data:1234

the topic `MyCmd` is configured in the example and should not be changed. The data `1234` can be changed to any string,
however it should be short enough to fit into a MQTT-SN message. 

The example program should be already running and the module should be in an attached state. So best wait, until some
temperature measurements have arrived in the Cloud. The state of the operation should change from **Pending** to
**Executing** to **Successful**. Before it changes from **Executing** to **Successful** a notification (new message)
should be presented by the running example program, similar to this:

    [ DEBUG    ] ----- New message 001 -----
    [----------] QoS:          1
    [----------] Id:           1
    [----------] Payload:      1234
    [----------] Topic:        NBIoT/111111111112345/CMD/MyCmd

## License

Copyright (C) 2018, Edgar Hindemith, Yassine Amraue, Thorsten Krautscheid, Kolja Vornholt, T-Systems International GmbH

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
See the License for the specific language governing permissions and
limitations under the License.
