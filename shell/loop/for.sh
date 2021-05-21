#!/usr/bin/env bash

for (( a = 1; a < 3; ++a )); do
    echo $a
done

for (( a = 1, b = 10; a < 5; ++a, --b )); do
    echo "$b - $a = $((b-a))"
done

for alphab in {a..z}; do
    echo -n $alphab
done
echo

var1=10
while [ $var1 -gt 0 ]; do
    echo $var1
    var1=$[ $var1 - 1 ]
done

path=/tmp/*
echo $path
for file in $path; do
    # echo $file
    if [ -d "$file" ]; then
        echo "$file is a directory"
        elif [ -f "$file" ]; then
        echo "$file is a file"
    fi
done
echo '-------------'

file=./for.sh
i=1
for line in $(cat $file);do
    echo $i $line
    i=$[ $i + 1 ]
done
