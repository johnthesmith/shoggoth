#!/bin/bash

./shab begin

# Install libraries
#./shab header "Check libraries"
#./shab apt "libglfw3"
#./shab apt "libglfw3-dev"
#./shab apt "libglu1-mesa-dev"
#./shab apt "upx"

# Building object files
./shab header "Obj building"
./shab build "hello.cpp"
./shab build "lib/*.cpp"
./shab build "lib/shoggoth/*.cpp"
./shab build "lib/graph/*.cpp"
./shab build "app/*.cpp"

# Make executable file
./shab header "make"
./shab execute "g++ -O3 -o hello OBJECT_PATH/*.o -lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw"

./shab header "compress"
./shab execute "upx -9 hello"

./shab header "run"
./shab execute "./hello"
