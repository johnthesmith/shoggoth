#!/bin/bash

./shab begin

./shab header "Obj building"

./shab build "hello.cpp"
./shab build "lib/*.cpp"
./shab build "lib/neuron/*.cpp"
./shab build "app/*.cpp"

./shab header "make"

./shab execute
g++ \
-O3 \
-o \
hello \
.shab/obj/*.o \
-lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw

./shab execute "upx -9 hello"

./shab execute "./hello"
