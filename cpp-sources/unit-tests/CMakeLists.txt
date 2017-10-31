cmake_minimum_required(VERSION 2.8)

project(caustic-unit-tests)

find_package (GTest REQUIRED)

include_directories (
    ${GTEST_INCLUDE_DIR}
    ../universal-device/high-level/include
    ../universal-device/high-level/high-level/glcd/devices/inc
    ../universal-device/high-level/high-level/glcd/controllers
    ../universal-device/high-level/high-level/glcd/fonts
    ../universal-device/high-level/high-level/glcd
    ../universal-device/high-level/high-level/lasertag-ssp/ssp
    ../universal-device/high-level/high-level/lasertag-ssp/ssp/Inc
    ../universal-device/low-level/Middlewares/Third_Party/FatFs/src
    ../universal-device/low-level/Middlewares/Third_Party/FatFs/src/drivers
)

set(EXE_SOURCES
    rcsp-aggregator-tests.cpp
    
    mock/network-layer-mock.cpp
    mock/loggers-mock.cpp
    mock/network-layer-mock.hpp


    ../universal-device/high-level/src/core/string-utils.cpp
    ../universal-device/high-level/src/rscp/RCSP-aggregator.cpp
    ../universal-device/high-level/src/rscp/RCSP-stream.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
)

add_test(NAME Library_testing
         COMMAND ${PROJECT_NAME})

run_test(${PROJECT_NAME})
