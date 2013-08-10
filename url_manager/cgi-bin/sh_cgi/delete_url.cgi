#!/bin/sh

set -x

echo "Content-type: text/html"
echo
echo

if [ ! -f $urls ]; then
    echo "<p>Url file does not exist. `pwd` </p>"
fi

len_str=`echo $QUERY_STRING | cut -d'&' -f1`
md5_str=`echo $QUERY_STRING | cut -d'&' -f2`
len=`echo $len_str | cut -d'=' -f2`
md5=`echo $md5_str | cut -d'=' -f2`

last2char=${md5: -2}

block_size=50
dump="../data/block_dump"
f=${dump}/$(($len % $block_size))
ff=${f}/dump_d_${last2char}

### dump is not deleted
fd=${f}/dump

echo $ff

echo "<p>Delete md5: " $md5 "</p>"

fgrep -q $md5 $ff && sed -i /`echo $md5`/d $ff
if [ $? -eq 0 ]; then
    echo "<p>Delete succeed.</p>"
    ./add_url_cnt.sh -1
else
    echo "<p>Delete failed.</p>"
fi

