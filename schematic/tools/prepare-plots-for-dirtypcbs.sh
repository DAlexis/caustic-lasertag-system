#!/bin/bash

(
    cd $1
    drill=`ls | grep .drl`
    txt="${drill/drl/txt}"
    cuts_gm1=`ls | grep .gm1`
    cuts_gbr="${cuts_gm1/gm1/gbr}"
    mv $drill $txt
    mv $cuts_gm1 $cuts_gbr
    zip $2 *
)
