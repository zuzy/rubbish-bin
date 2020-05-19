#!/bin/bash

for i in /sys/block/*/dev sys/block/*/*/dev
do
    if [ -f $i ]
    then
        MAJOR=$(sed 's/:.*//' < $i)
        MINOR=$(sed 's/.*://' < $i)
        DEVNAME=$(echo $i | sed -e 's@/dev@@' -e 's@.*/@@')
        echo /dev/$DEVNAME b $MAJOR $MINOR
    fi
done
