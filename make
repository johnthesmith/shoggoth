#!/bin/bash

shab="../shab/shab"

##Clear logs
#echo "" >> teacher.log
#echo "" >> server.log
#echo "" >> ui.log
#echo "" >> processor.log
#echo "" >> evolution.log


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
$shab header "Objects files building"
$shab build "../../lib/core/*.cpp"
$shab build "../../lib/json/*.cpp"
$shab build "../../lib/sock/*.cpp"
$shab build "../../lib/graph/*.cpp"
$shab build "src/shoggoth/*.cpp"
$shab build "src/shoggoth/limb/*.cpp"
$shab build "src/app/*.cpp"

## Make executable file
$shab header "make shoggoth"
$shab execute "COMPILER -O3 -o shoggoth $(grep -v '^#' shoggoth.objs) -lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw -lMagick++"
$shab header "make evolution"
$shab execute "COMPILER -O3 -o evolution $(grep -v '^#' evolution.objs) -lpthread"

#$shab header "compress"
#$shab execute "upx -9 shoggoth"
#$shab execute "upx -9 evolution"
