#!/bin/bash

(
    cd $1
    drill=`ls | grep drl`
    txt="${drill/drl/txt}"
    mv $drill $txt
    zip $2 *
)
