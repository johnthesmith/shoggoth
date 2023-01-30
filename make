#!/bin/bash

./shab begin

./shab header "Obj building"

./shab build "hello.cpp"
./shab build "lib/*.cpp"
./shab build "lib/neuron/*.cpp"
./shab build "app/*.cpp"

./shab header "make"
./shab execute "g++ -O3 -o hello OBJECT_PATH/*.o -lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw"

./shab header "compress"
./shab execute "upx -9 hello"

./shab header "run"
./shab execute "./hello"
