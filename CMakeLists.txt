cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(MQTT_Protobuf CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#include(conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Protobuf REQUIRED)
add_subdirectory(Protos_Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Mqtt Xml)

add_executable(${PROJECT_NAME}
    machinehandle.h
    machinehandle.cpp
    mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    main.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC CONAN_PKG::protobuf
    PUBLIC CONAN_PKG::pugixml
    Protos_Example
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Mqtt
    Qt6::Xml
)

include_directories(${CMAKE_BINARY_DIR} ${ProtobufMessagesIncludePath} ${Protobuf_INCLUDE_DIRS})

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

#Comment out to disable testing
include(CTest)
enable_testing()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
