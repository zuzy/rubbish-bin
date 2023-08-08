#!/usr/bin/env bash
# https://stackoverflow.com/questions/23048702/bash-while-loop-threading
# https://stackoverflow.com/questions/1683976/multi-threaded-bash-programming-generalized-method

set -eu -o pipefail

parallel=$(( $(nproc) / 3 ))
# parallel=$(nproc)
echo $parallel

pids=()
thread() {
    set +e
    local this
    #   while [ ${#} -gt 6 ]; do
    #   while [ ${#} -gt $num ]; do
    while [ ${#} -ge $parallel ]; do
        this=${1}
        wait "$this"
        shift
    done
    # pids=($1 $2 $3 $4 $5 $6)
    pids=($@)
    # echo " pids: ${pids[@]}"
    # echo "\$@ $@"
    # echo "\$# $#"
}

i=0
delay=0
test_func() {
    echo "$i start"
    sleep $delay
    python -m pip config list
    echo "$i end"
}

for i in 1 2 3 4 5 6 7 8 9 10
do
    delay=1
    test_func &
    pids=( ${pids[@]-} $(echo $!) )
    thread "${pids[@]}"
done
for pid in ${pids[@]}
do
    wait "$pid"
done

set -e
for i in 1 2 3 4 5 6 7 8 9 10
do
    python -m pip config list
done