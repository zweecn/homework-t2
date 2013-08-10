#!/bin/bash

# set -x

source ./shell/s_var_export.sh

in_htmls=$1
out_htmls=$2

while read htm
do
    md5_t=`echo $htm | md5sum | cut -d ' ' -f 1`
    fgrep -q $md5_t $unvisited
    if [ $? -ne 0 ]; then
        echo "${htm}	$md5_t" >> $out_htmls
    fi
done < $in_htmls

