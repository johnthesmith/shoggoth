#!/bin/bash

./shab begin


# Upload libraries
#./shab git "asdasdasdasdsa"

# Install aprt
#./shab header "Check libraries"
#./shab apt "libglfw3"
#./shab apt "libglfw3-dev"
#./shab apt "libglu1-mesa-dev"
#./shab apt "upx"

# Building object files
./shab header "Obj building"
./shab build "shoggoth.cpp"
./shab build "src/lib/*.cpp"
./shab build "src/shoggoth/*.cpp"
./shab build "src/graph/*.cpp"
./shab build "src/app/*.cpp"

# Make executable file
./shab header "make"
./shab execute "g++ -O3 -o shoggoth OBJECT_PATH/*.o -lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw"

./shab header "compress"
./shab execute "upx -9 shoggoth"

./shab header "run"
./shab execute "./shoggoth"
