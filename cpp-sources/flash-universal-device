#!/bin/bash

dir="debug"

if [ "$1" == "release" ];
then
    dir="release"
fi

st-flash write build/$dir/universal-device/universal-device.bin 0x8000000 
