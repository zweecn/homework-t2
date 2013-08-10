#!/bin/bash

source ./shell/s_var_export.sh

awk -F '\t' '{
    if (!mark[$2]) {
        mark[$2] = 1;
        print;
    }
}' $unvisited > ${unvisited}_uniq
