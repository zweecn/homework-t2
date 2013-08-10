#!/bin/bash

# set -x

source ./shell/s_var_export.sh

msg "Spider lite begin..."

if [ $# -eq 1 ]; then
    ./shell/s_root_download.sh $1
    if [ $? -ne 0 ]; then
        msg "Root URL download failed."
        exit 1 
    fi
else
    echo "Usage: ./shell/spider_ctl.sh qq.com"
    exit 1
fi

i=1
while read url md5
do
    msg "Begin ${i}th downloading..."
    url_path="${outdir}$i"

    ./shell/s_download.sh $i $url $md5 $url_path
    if [ $? -ne 0 ]; then
        msg "Download $url failed."
        continue
    else
        echo "${url}	$md5	${url_path}" >> $visited
    fi

    ./shell/s_search_urls.sh $url_path ${tmp}_htm
    if [ $? -ne 0 ]; then
        msg "Search urls failed"
        continue
    fi

    ./shell/s_uniq_url.sh ${tmp}_htm ${tmp}_htm_uniq
    if [ $? -ne 0 ]; then
        msg "Uniq urls failed"
        continue
    fi

    cat ${tmp}_htm_uniq >> $unvisited
    if [ $? -ne 0 ]; then
        msg "Append urls to unvisited failed"
        continue
    fi

    ((i++))
    sleep 1
done < $unvisited

msg "Spider lite finished."

