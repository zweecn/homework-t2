#!/bin/bash

# set -x

source ./shell/s_var_export.sh

i=$1
url=$2
md5=$3
url_path=$4

fgrep -q $md5 $download_md5
if [ $? -eq 0 ]; then
    msg "$url is visited."
    exit 1 
fi

url_path="${outdir}$i"
msg "Downloading $url to $url_path ..."
wget -O ${url_path} -T 6 -t 2 $url -- spider >> $download_log 2>&1
d_md5=`echo $url | md5sum | cut -d ' ' -f 1`
echo "$url	$d_md5">> $download_md5

filetype=`file $url_path` 
echo $filetype | grep -q "text" 
if [ $? -ne 0 ]; then
    msg "remove $url_path : $filetype"
    rm -rf $url_path
    exit 1 
fi

htm=`cat $url_path`
if [ -z $url_path ]; then
    msg "`date` $url_path is empty."
    exit 1    
fi

