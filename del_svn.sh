#!/bin/bash

for s in `find . -name .svn`
do
	echo $s
	rm -rf $s
done
