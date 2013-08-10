#!/bin/sh

urls="../url_md5"

set -x

echo "Content-type: text/html"
echo
echo

if [ "$REQUEST_METHOD" = "POST" ] ; then
    QUERY_STRING=`cat -`
fi

url=`echo $QUERY_STRING | cut -d'=' -f2-`
len=`echo $url | awk '{print length($1);}'`
md5=`echo $url | md5sum | cut -d' ' -f1`
last2char=${md5: -2}

block_size=50
dump="../data/block_dump"
f=${dump}/$(($len % $block_size))
ff=${f}/dump_d_${last2char}

###fd=${f}/dump

echo "<p>Add url:" $url " <br/> to file:" $ff " </p>"

fgrep -q "$url" $ff
if [ $? -eq 0 ]; then
    echo "<p>Url exist.</p>"
    exit 0
fi

echo "${url}	${md5}" >> $ff
if [ $? -eq 0 ]; then
    echo "<p>Add succeed.</p>"
    ./add_url_cnt.sh 1
else
    echo "<p>Add failed.</p>"
fi

