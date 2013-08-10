#!/bin/bash


for p in `ps aux| grep read_proc | cut -d ' ' -f6`
do 
	kill $p
done

for p in `ps aux | grep storage_server | cut -d ' ' -f6` 
do
	kill $p
done

for p in `ps aux | grep transfer.exe | cut -d ' ' -f6`
do
	kill $p 
done

user=`whoami`

for  m in `ipcs -m -c | grep $user | cut -d ' ' -f 1`
do
	ipcrm -m $m
done

for  s in `ipcs -s -c | grep $user | cut -d ' ' -f 1`
do
	ipcrm -s $s
done

exit 0
