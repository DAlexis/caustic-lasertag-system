#!/bin/bash

dir="debug"

if [ "$1" == "release" ];
then
    dir="release"
fi

st-flash write build/$dir/new-universal-device/new-universal-device.bin 0x8000000 
