#!/bin/bash

# set -x

source ./shell/s_var_export.sh

url_filepath=$1
out_urls=$2

grep -oP "[a-zA-z]+://[^\s\"\'\)]*" $url_filepath | sort | uniq > $out_urls

