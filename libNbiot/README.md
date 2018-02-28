## Integration of the PAHO MQTT-SN Implementation

Currently, the mqtt-sn client and protocol of the Eclipse Paho project are included in these sources.
The protocol implementation can be found in

`src/MQTTSNPacket`

All files found there are plain C-files, which need to be compiled with C++. For this reason the C-files
with file ending **.c** have been renamed to C++-files with line ending **.cpp**.


## Troubleshooting:

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


