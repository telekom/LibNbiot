cmake_minimum_required(VERSION 3.0)
project(LibNbiot)

set(CMAKE_CXX_STANDARD 11)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external)

include_directories(include)
include_directories(${EXTERNAL_DIR}/src/FP)
include_directories(${EXTERNAL_DIR}/src/List)
include_directories(${EXTERNAL_DIR}/src/MQTTSNClient)
include_directories(${EXTERNAL_DIR}/src/MQTTSNPacket)
include_directories(${EXTERNAL_DIR}/src/StateMachine)

set(SOURCE_FILES
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNConnectClient.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNConnectServer.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNDeserializePublish.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNPacket.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNSearchClient.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNSearchServer.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNSerializePublish.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNSubscribeClient.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNSubscribeServer.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNUnsubscribeClient.c
        ${EXTERNAL_DIR}/src/MQTTSNPacket/MQTTSNUnsubscribeServer.c
        ${EXTERNAL_DIR}/src/StateMachine/abstractstatemachine.cpp
        ${EXTERNAL_DIR}/src/StateMachine/choice.cpp
        ${EXTERNAL_DIR}/src/StateMachine/eventdispatcher.cpp
        ${EXTERNAL_DIR}/src/StateMachine/state.cpp
        ${EXTERNAL_DIR}/src/StateMachine/stmevent.cpp
        ${EXTERNAL_DIR}/src/StateMachine/transition.cpp
        src/libnbiot.cpp
        src/nbiotmqtt.cpp
        src/nbiotstm.cpp
        src/nbiottopicregistry.cpp)

add_library(LibNbiot STATIC ${SOURCE_FILES})

set_target_properties(LibNbiot PROPERTIES OUTPUT_NAME "nbiot")
set_target_properties(LibNbiot PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib)