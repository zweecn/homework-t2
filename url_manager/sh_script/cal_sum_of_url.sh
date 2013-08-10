#!/bin/bash

block_size=50
dump="../cgi-bin/data/block_dump"

sum_of_file=0
sum_of_url=0

for ((i=0; i<$block_size; i++))
do
    f=${dump}/$(($i % $block_size))
    for f_name in `ls ${f}/dump_d_*`
    do
        line=`wc -l $f_name | cut -d' ' -f1`
        sum_of_file=$(($sum_of_file + 1))
        sum_of_url=$((sum_of_url + $line))
    done 
done

echo "Sum of file:" $sum_of_file 
echo "Sum of url:" $sum_of_url
