#!/usr/bin/env bash

function usage() {
    echo $1 mac-list-file [target-directory]
}

if [ $# -lt 1 ]; then
    usage $0
fi
macList=$1

if [ ! -e $macList ]; then
    usage $0
fi

if [ $# -ge 2 ]; then
    targetDir=$2
else
    targetDir=./
fi

if [ ! -d $targetDir ]; then
    echo "Target directory $targetDir"
    mkdir -p $targetDir
fi

index=1
for line in $(cat $macList); do
# cat $macList | while read line; do
    echo $line | sed 's/://g' | sed 's/\([0-9a-fA-F]\{2\}\)/\\\\\\x\1/gI' | xargs echo -e -n > $targetDir/mac_$index.bin
    echo $line | sed 's/://g' | sed 's/[0-9a-fA-F][0-9a-fA-F]/\\x&/g'| xargs -e printf
    printf "${mac}"
    # echo $line | sed 's/://g' | sed 's/[0-9a-fA-F][0-9a-fA-F]/\\x&/g' | xxd -r -p > $targetDir/mac_$index.bin
    # echo $line | sed 's/://g' | sed 's/[0-9a-fA-F][0-9a-fA-F]/\\x&/g' | xxd -r -p > $targetDir/mac_$index.bin
    # echo $line | sed 's/://g' | sed 's/[0-9a-fA-F][0-9a-fA-F]/\\x&/g' | xargs -i printf "%b" {} > file.bin
    # echo $line | sed 's/://g' | sed 's/[0-9a-fA-F][0-9a-fA-F]/\\x&/g' | xargs -i echo -e '{}'

    # echo $line | sed 's/://g' | sed 's/[0-9a-fA-F][0-9a-fA-F]/\\x&/g' | xargs -i echo -n -e '"{}"' > $targetDir/mac_$index.bin
    index=$((index+1))
done