#!/usr/bin/env bash
# https://stackoverflow.com/questions/23048702/bash-while-loop-threading
# https://stackoverflow.com/questions/1683976/multi-threaded-bash-programming-generalized-method

parallel=$(( $(nproc) / 3 ))
# parallel=$(nproc)
echo $parallel

pids=()
thread() {
    local this
    #   while [ ${#} -gt 6 ]; do
    #   while [ ${#} -gt $num ]; do
    while [ ${#} -ge $parallel ]; do
        # this=${1}
        # ps $this
        # wait "$this"
        # shift
        sleep 1
    done
    # pids=($1 $2 $3 $4 $5 $6)
    pids=($@)
    # echo " pids: ${pids[@]}"
    # echo "\$@ $@"
    # echo "\$# $#"
}

for i in ${!list[@]};do
    if [ "${list[$i]}" == "$delete_me" ]; then
        unset list[$i]
    fi 
done

waitpids() {
    local p=$_pid
    wait "$p"
    for i in ${!pids[@]};do
        if [ "${pids[$i]}" == "$p" ]; then
            unset pids[$i]
        fi
    done
}

i=0
delay=0
test_func() {
    echo "$i start"
    sleep $delay
    echo "$i end"
}

max=60
for i in 1 2 3 4 5 6 7 8 9 10
do
    # (echo "$i start" && sleep 3 && echo "$i end")&
    # delay=$(( $i - $(( $max / 2 )) ))
    # delay=$(( $max - ( $i / 2 ) * 10 ))
    delay=5
    echo "delay $delay"
    test_func &
    # pids=( ${pids[@]-} $(echo $!) )
    _pid=$(echo $!)
    echo $_pid
    pids=( ${pids[@]-} $(echo $!) )
    pids=( ${pids[@]-} $_pid )
    waitpids &
    # thread 
    thread ${pids[@]}
done
for pid in ${pids[@]}
do
    wait "$pid"
done