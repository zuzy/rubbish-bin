#!/usr/bin/env bash

declare -a cmd_array=("umd" "tools" "demo1" "inference" "erpcgen" "erpclib")

function _contain () {
    local match="$1"
    for e in "${cmd_array[@]}"; do [[ "$e" == "$match" ]] && return 0; done
    return 1
}

function _all() {
    for c in "${cmd_array[@]}"; do
        echo $c
        # $(b_"$c")
    done
}

# _contain $1
# if [[ $? -eq 0 ]] ; then
#     echo yes
# else
#     echo not
# fi


for cmd in $@; do
    _contain $cmd
    if [[ $? -eq 0 ]]; then
        echo has $cmd
    else
        # if [[ "${cmd_array[@]}" =~ "$cmd" ]]; then
        #     echo has $cmd
        # else
        case $cmd in
            sample) echo support $cmd;;
            erpc) echo support $cmd;;
            all) echo $cmd;_all;;
            *) echo unsupport $cmd;
        esac
    fi
done
