#!/bin/bash

conf="conf/ip_port"
ip=`fgrep "ip" $conf | cut -d' ' -f2`
port=`fgrep "port" $conf | cut -d' ' -f2`


### ²âÊÔ A ¼ÇÂ¼
nslookup -type=a -port=$port www.oa.com $ip 

### ²âÊÔ PTR ¼ÇÂ¼
nslookup -type=ptr -port=$port 10.10.10.10 $ip 

