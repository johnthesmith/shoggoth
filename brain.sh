#!/bin/bash

./brain \
--net_id=alpha \
--net_version="000000.000000.000000" \
--config=./net/alpha/brain.json \
--mon=./net/alpha/mon/brain_main.json

#gdb --args ./brain --net_id=alpha --net_version=000000.000000.000000 --config=./net/alpha/brain.json --mon=./net/alpha/mon/brain_main.json
#<<EOF
#set environment ASAN_OPTIONS=halt_on_error=0
#handle SIGSEGV nostop noprint pass
#run
#EOF
