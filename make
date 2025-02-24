#!/bin/bash

shab="../shab/shab"

##Clear logs
#echo "" >> teacher.log
#echo "" >> server.log
#echo "" >> ui.log
#echo "" >> brain.log
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
$shab build "src/app/teacher/*.cpp"
$shab build "src/app/evolution/*.cpp"
$shab build "src/app/brain/*.cpp"
#$shab build "src/app/ui/*.cpp"

## Make executable file
# $shab header "make shoggoth"
# Enable sanitizer memory
# -fsanitize=address

$shab header "make evolution"
$shab execute "COMPILER -g -O3 -o evolution $(grep -v '^#' evolution.objs) -lpthread"

$shab header "make teacher"
$shab execute "COMPILER -g -O3 -o teacher $(grep -v '^#' teacher.objs) -lpthread -lMagick++"

$shab header "make brain"
$shab execute "COMPILER -g -O3 -o brain $(grep -v '^#' brain.objs) -lpthread"

#$shab header "make ui"
#$shab execute "COMPILER -g -O3 -o ui $(grep -v '^#' ui.objs) -lpthread"

#$shab header "compress"
#$shab execute "upx -9 shoggoth"
#$shab execute "upx -9 evolution"
#$shab execute "upx -9 teacher"
#$shab execute "upx -9 ui"
