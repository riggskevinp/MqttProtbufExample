cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Test LANGUAGES C CXX)

#set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
#find_library(ScannerUtils NAMES ScannerUtils)
include_directories(..)

add_executable(Test
        commsTests.cpp
        ../machinehandle.h
        ../machinehandle.cpp
        )

gtest_discover_tests(Test)

target_link_libraries(Test
        PUBLIC CONAN_PKG::pugixml
        gtest
        gtest_main
)

add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/templatetest.xml
                ${CMAKE_BINARY_DIR}/bin/templatetest.xml
)

