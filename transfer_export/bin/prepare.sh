#!/bin/bash

./shm_app

./read_proc > read_proc.log 2>&1 &

./storage_server > storage_server.log 2>&1 &

./transfer.exe > transfer.log 2>&1 &

exit 0
