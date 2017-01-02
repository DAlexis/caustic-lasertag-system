cmake_minimum_required(VERSION 2.8)

project(Caustic-lasertag-system)

if (EXISTS "${PROJECT_SOURCE_DIR}/bootloader-usb/bootloader-usb/CMakeLists.txt")
    add_subdirectory(bootloader-usb/bootloader-usb)
endif()

add_subdirectory(universal-device)


