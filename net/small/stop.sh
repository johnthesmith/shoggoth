#!/bin/bash

while read pid; do
    pkill -P $pid 2>/dev/null
    kill $pid 2>/dev/null
done < pids.txt

rm -f pids.txt
