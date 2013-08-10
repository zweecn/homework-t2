#!/bin/bash

urls="data/dump_url"

echo "`date` Begin..." $1

block_size=50
dump="data/block_dump"

i=$1
f="${dump}/$i/dump"

while read url cnt
do
    md5=`echo $url | md5sum | cut -d' ' -f1`
    last2char=${md5: -2}
    ff="$f"_d_"$last2char"
    echo "$url	$md5" >> $ff
done < $f

echo "`date` Finished."

