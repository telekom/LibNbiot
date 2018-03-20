cmake_minimum_required(VERSION 3.0)
project(LibNbiotCore)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(SOURCE_FILES
        src/atcommands.cpp
        src/libnbiotcore.cpp
        src/nbiotconnectivity.cpp
        src/nbiotcoreapp.cpp
        src/nbiotdebug.cpp
        src/nbiotmodemtools.cpp
        src/nbiotstring.cpp
        src/nbiottime.cpp
        src/nbiottimer.cpp
        src/serial.cpp)

add_library(LibNbiotCore STATIC ${SOURCE_FILES})

set_target_properties(LibNbiotCore PROPERTIES OUTPUT_NAME "nbiotcore")
set_target_properties(LibNbiotCore PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
