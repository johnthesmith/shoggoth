#!/bin/bash

./shab begin


# Upload libraries
#./shab git ""

# Install aprt
#./shab header "Check libraries"
#./shab apt "libglfw3"
#./shab apt "libglfw3-dev"
#./shab apt "libglu1-mesa-dev"
#./shab apt "upx"

# Building object files
./shab header "Obj building"
./shab build "src/lib/*.cpp"
./shab build "src/json/*.cpp"
./shab build "src/graph/*.cpp"
./shab build "src/shoggoth/*.cpp"
./shab build "src/app/*.cpp"

# Make executable file
./shab header "make"
./shab execute "g++ -O3 -o shoggoth OBJECT_PATH/*.o -lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw -lMagick++"
./shab header "compress"
./shab execute "upx -9 shoggoth"

# Run
#./shab header "run"
#./shab execute "./shoggoth --config=test/server.json --log=server.log --htopdebug" &
#./shab execute "./shoggoth --config=test/processor.json --log=processor.log --htopdebug"
#./shab execute "./shoggoth --config=test/ui.json --log=ui.log --htopdebug"
#./shab execute "./shoggoth --config=test/teacher.json --log=teacher.log"
