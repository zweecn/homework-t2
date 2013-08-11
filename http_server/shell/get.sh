#!/bin/bash


while read url
do
    wget -P data $url 
done < conf/index
