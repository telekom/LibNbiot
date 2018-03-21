project(LibNbiotOS)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

include_directories(libNbiotCore/include)
include_directories(libNbiot/include)
include_directories(external/src/FP)
include_directories(external/src/List)
include_directories(external/src/MQTTSNClient)
include_directories(external/src/MQTTSNPacket)
include_directories(external/src/StateMachine)
include_directories(libNbiot/src)

add_subdirectory(libNbiotCore)
add_subdirectory(libNbiot)