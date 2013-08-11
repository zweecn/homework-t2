#!/bin/bash

f_cnt=2000
min_size=16
max_size=64

dir="wwwroot"
port=8888

ip=`/sbin/ifconfig | grep "inet addr" | cut -d ':' -f2 | cut -d' ' -f1 | head -n 1`

./bin/make_file $f_cnt $min_size $max_size $dir | awk -F '/' '{
    print "http://" "'$ip'" ":" "'$port'" "/" $2;
}'
