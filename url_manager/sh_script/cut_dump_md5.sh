#!/bin/bash

urls="data/dump_url"

echo "`date` Begin..."

### Begin divide urls...

block_size=50
dump="data/block_dump"

### Make dir
rm -rf ${dump}
for((i=0; i<$block_size; i++))
do
    echo $i
    mkdir -p ${dump}/$i
done

### Divide url
awk -F '\t' '{
    len = length($1);
    p = len % "'$block_size'";
    print $1 "\t" p > "'$dump'" "/" p "/dump"; 
} END {
}' $urls 

### Cal lines of every dump
for((i=0; i<$block_size; i++))
do
    f="${dump}/$i/dump"
    wc -l $f
done

### Create muti progrecess to divite urls
for((i=0; i<$block_size; i++))
do
    ./script/md5_divide.sh $i > ./logs/md5_divide.log 2>&1 &
done

echo "`date` Finished."

