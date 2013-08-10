#!/bin/bash

set -x

url_cnt="../data/url_cnt"

delta=$1

awk '{
    url = $1;
    url += "'$delta'";
    print url;

}' ${url_cnt} > ${url_cnt}_tmp

mv ${url_cnt}_tmp ${url_cnt}

chmod 777 $url_cnt

