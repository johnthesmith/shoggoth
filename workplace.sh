#!/bin/bash

SLEEP=0.05
PROJECT="~/code/c/app/shoggoth/"
PROJECT_LIB="~/code/c/lib/"

# Переключаемся на 5-е рабочее место
i3-msg workspace 5

sleep $SLEEP

i3-msg focus parent
i3-msg focus parent
i3-msg focus parent

i3-msg kill


# Запускаем первое окно
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT_LIB; mc; bash" && sleep $SLEEP

## Устанавливаем вид закладок
i3-msg layout tabbed

## Переходим на первое открытое окно
i3-msg focus left
i3-msg focus left
i3-msg focus left
i3-msg focus left
i3-msg focus left
i3-msg focus left

i3-msg split h
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP

i3-msg focus right
i3-msg split h
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP

i3-msg focus right
i3-msg split h
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP

i3-msg focus right
i3-msg split h
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP

i3-msg focus right
i3-msg split h
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP

i3-msg focus right
i3-msg split h
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP

i3-msg focus right
i3-msg split h
terminator -e "cd $PROJECT_LIB; mc; bash" && sleep $SLEEP

terminator -e "cd $PROJECT; bash" && sleep $SLEEP
i3-msg move down
i3-msg resize set 100 ppt 20 ppt





# Переключаемся на 4-е рабочее место
i3-msg workspace 4

sleep $SLEEP

i3-msg focus parent
i3-msg focus parent
i3-msg focus parent

i3-msg kill


# Запускаем первое окно
terminator -e "nano $PROJECT/net/alpha/ver/zero/net.json" && sleep $SLEEP
terminator -e "nano $PROJECT/net/alpha/brain.json" && sleep $SLEEP
terminator -e "nano $PROJECT/net/alpha/evolution.json" && sleep $SLEEP
terminator -e "nano $PROJECT/net/alpha/teacher.json" && sleep $SLEEP
i3-msg layout tabbed


terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/brain_main.json" && sleep $SLEEP
i3-msg move right
i3-msg split h
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/brain_server.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/brain_processor.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/teacher_app.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/procesor.json" && sleep $SLEEP
i3-msg layout tabbed



terminator -e "cd $PROJECT; bash" && sleep $SLEEP
i3-msg move down
i3-msg resize set 100 ppt 20 ppt
i3-msg split h
terminator -e "cd $PROJECT; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; bash" && sleep $SLEEP
