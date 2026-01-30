#!/bin/bash

./brain.sh &
echo $! > pids.txt

./teacher.sh &
echo $! >> pids.txt

./evolution.sh &
echo $! >> pids.txt
