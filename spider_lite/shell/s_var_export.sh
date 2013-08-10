#!/bin/bash

# set -x

export outdir="data/dump_urls/"

export visited="data/visited"
export unvisited="data/unvisited"
export download_md5="data/download_md5"

export logs="logs"
export log="logs/spider.log"
export download_log="logs/download.log"

export tmpdir="data/tmp/"
export tmp="data/tmp/spider.tmp"

msg()
{
    echo "`date`	$1	$0" >> $log;
}

test -d $outdir || mkdir -p $outdir
test -d $logs || mkdir -p $logs
test -d $tmpdir || mkdir -p $tmpdir

test -f $download_md5 || touch $download_md5
test -f $visited || touch $visited
test -f $unvisited || touch $unvisited

