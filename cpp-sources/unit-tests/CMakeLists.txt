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
)

set(EXE_SOURCES
    unit-test-stub.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
)

add_test(NAME Library_testing
         COMMAND ${PROJECT_NAME})

run_test(${PROJECT_NAME})
