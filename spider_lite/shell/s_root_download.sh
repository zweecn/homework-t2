#!/bin/bash

# set -x

source ./shell/s_var_export.sh

if [ $# -eq 1 ]; then
    root_url=$1
    msg "Root url is: $root_url"
    
    md5=`echo $root_url | md5sum | cut -d ' ' -f 1`
    echo "$root_url	${md5}" > ${unvisited}
else
    msg "Usage: ./s_root_download.sh qq.com"
    msg "Exit."
    exit 1
fi

