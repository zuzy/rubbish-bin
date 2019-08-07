#!/bin/bash
val=true
sleep 3;
while true; do
    val_wlo=`ifconfig -a | grep wlo`
    # echo $val_wlo
    if $val
    then
        if test '$val_wlo'
        then
            echo 'yes!'
            val=false;
        else
            echo 'no'
        fi
    else
        if !(test '$val_wlo')
        then
            echo 'hello!'
            val=true;
        else
            echo 'nono!'
        fi
    fi
    sleep 2
done