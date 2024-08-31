#!/bin/bash
./shoggoth \
--net_id=alpha \
--net_version="zero" \
--config=./net/alpha/processor.json \
#--log=./net/alpha/log/processor.log \
--mon=./net/alpha/mon/processor_main.json
