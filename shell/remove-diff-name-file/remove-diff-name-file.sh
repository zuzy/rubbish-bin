#!/usr/bin/env bash

# files=test/*
files=`find test/ -type f`

for f in $files; do
    fn=${f%.*}
    if [ -e "$fn.a" ]; then
        echo "Exist $fn.a"
    fi
    if [ ! -e "$fn.b" ]; then
        echo "Not exist $fn.b"
    fi
    echo " " $f "${f%.*}"
done
