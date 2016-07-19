#!/bin/bash

dir="debug"

if [ "$1" == "release" ];
then
    dir="release"
fi

st-flash write build/$dir/bootloader-usb/bootloader-usb/usb-bootloader.bin 0x8000000 
