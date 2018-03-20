cmake_minimum_required(VERSION 3.0)
project(LibNbiotOS)

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