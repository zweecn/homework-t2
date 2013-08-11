#!/bin/bash

f_cnt=20
wwwroot="wwwroot"

for((i=0; i<$f_cnt; i++))
do
    echo $RANDOM$RANDOMRANDOM$RANDOM$RANDOM > ${wwwroot}/$i
    echo $RANDOM$RANDOMRANDOM$RANDOM$RANDOM >> ${wwwroot}/$i
done
