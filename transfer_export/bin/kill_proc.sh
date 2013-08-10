#!/bin/bash


for p in `ps aux| grep read_proc | cut -d ' ' -f6`
do 
	echo $p
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

exit 0
