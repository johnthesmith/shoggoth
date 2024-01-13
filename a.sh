#!/bin/bash

for currentFile in `find ./ -type f \( -name "*.h" -o -name "*.cpp" \)`
do
    # return file
    currentFile=`realpath "$currentFile"`
    currentDir=`dirname "$currentFile"`

    echo $currentFile ==========================

    code=$(cat $currentFile | grep -E '#include \".+\"')
    while read -r line
    do
        lib=`echo "$line" | awk '{ print $2 }' | sed 's/\"//g'`
        echo - `realpath "$currentDir/$lib"`
    done <<< "$code"
done

