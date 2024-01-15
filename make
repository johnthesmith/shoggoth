#!/bin/bash

shab="../shab/shab"

#Clear logs
echo "" >> teacher.log
echo "" >> server.log
echo "" >> ui.log
echo "" >> processor.log

# Make begin
$shab begin

# Install apt
#./shab header "Check libraries"
#./shab install apt "
#libglfw3
#libglfw3-dev
#libglu1-mesa-dev
#libglew-dev
#imagemagick
#graphicsmagick-libmagick-dev-compat
#upx-ucl
#"

## Building object files
$shab header "Obj building"
$shab build "../../lib/core/*.cpp"
$shab build "../../lib/json/*.cpp"
$shab build "../../lib/sock/*.cpp"
$shab build "../../lib/graph/*.cpp"
$shab build "src/shoggoth/*.cpp"
$shab build "src/shoggoth/limb/*.cpp"
$shab build "src/app/*.cpp"

## Make executable file
$shab header "make"
$shab execute "g++ -O3 -o shoggoth OBJECT_PATH/*.o -lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw -lMagick++"
#$shab header "compress"
#$shab execute "upx -9 shoggoth"

# Run
#$shab header "run"
#$shab execute "./shoggoth --config=test/server.json --log=server.log --htopdebug" &
#$shab execute "./shoggoth --config=test/processor.json --log=processor.log --htopdebug"
#$shab execute "./shoggoth --config=test/ui.json --log=ui.log --htopdebug"
#$shab execute "./shoggoth --config=test/teacher.json --log=teacher.log"


