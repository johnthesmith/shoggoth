#!/bin/bash

echo -e '\033[0;32m=================================================================\033[0m'
echo -e '\033[0;32m Make \033[0m'
echo -e '\033[0;32m=================================================================\033[0m'


g++ \
hello.cpp \
lib/*.cpp \
app/*.cpp \
-lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw -o hello && \
./hello
