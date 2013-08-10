#!/bin/bash


./read_proc > log 2>&1 &

./storage_server > sto.log 2>&1 &

./transfer.exe > tran.log 2>&1 &

./client $1

exit 0
