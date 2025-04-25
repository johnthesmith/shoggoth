#!/bin/bash

SLEEP=0.05
PROJECT="~/src/c/app/shoggoth/"
PROJECT_LIB="~/src/c/lib/"

# Переключаемся на 6-е рабочее место
i3-msg workspace 6

sleep $SLEEP

i3-msg focus parent
i3-msg focus parent
i3-msg focus parent
i3-msg focus parent

i3-msg kill


# Создаем окно которе в дальнейшем будет конфигуратором отладчика
terminator -e "cd $PROJECT; echo Debuger; bash" && sleep $SLEEP
# Создаем окно которое в дальнейшем будет первм окном разработки
terminator -e "cd $PROJECT; nano README.md; bash" && sleep $SLEEP

i3-msg layout tabbed

# Смещаемся на дебагер
i3-msg focus left

# Разделяем экран горизонтально
i3-msg split v

# Создаем нижнее окно отладчика и два дополнительных
#i3-msg layout tabbed
terminator -e "cd $PROJECT; bash" && sleep $SLEEP
i3-msg resize set 100 ppt 20 ppt
i3-msg split h
terminator -e "cd $PROJECT; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; bash" && sleep $SLEEP

# перемещаемся на оно с конфигурациями
i3-msg focus up
i3-msg split h

# Создаем окно мониторингов
terminator -e "cd $PROJECT; bash" && sleep $SLEEP

# Возвращаемся на окно конфигурацтоа и добавляем новые окна конфигуации
i3-msg focus left
i3-msg split h
i3-msg layout tabbed
## Запускаем окна конфигов
terminator -e "nano $PROJECT/net/alpha/ver/zero/net.json" && sleep $SLEEP
terminator -e "nano $PROJECT/net/alpha/brain.json" && sleep $SLEEP
terminator -e "nano $PROJECT/net/alpha/evolution.json" && sleep $SLEEP
terminator -e "nano $PROJECT/net/alpha/teacher.json" && sleep $SLEEP

# Переключаемся на правое окно с мониторингами
i3-msg focus right
i3-msg split h
i3-msg layout tabbed
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/brain_main.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/brain_server.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/brain_processor.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/teacher_app.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/procesor.json" && sleep $SLEEP
terminator -e "watch -n 0.1 jq -C . $PROJECT/net/alpha/mon/evolution_main.json" && sleep $SLEEP
terminator -e "cd $PROJECT/net/alpha/; ./values.sh" && sleep $SLEEP
i3-msg split h
terminator -e "cd $PROJECT/net/alpha/; ./errors.sh" && sleep $SLEEP




###############################################################################
# Разработка
i3-msg focus right
i3-msg split v

# Создание окна компиляции
terminator -e "cd $PROJECT; bash" && sleep $SLEEP
i3-msg resize set 100 ppt 20 ppt && sleep $SLEEP

i3-msg focus up
i3-msg split h

# Запускаем первое окно
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT; mc; bash" && sleep $SLEEP
terminator -e "cd $PROJECT_LIB; mc; bash" && sleep $SLEEP

# Устанавливаем вид закладок
i3-msg layout tabbed

# Переходим на первое открытое окно
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
