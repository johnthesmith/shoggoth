#!/bin/bash

# Compiler
CPP='g++ -c SOURCE_FILE -o ./obj/OBJECT_FILE'

# Cache path define
CachePath="./cache"

# Object file path define
ObjPath="./obj"

# Colors
COLOR_DEFAULT="\033[0m"
COLOR_RED="\033[0;31m"
COLOR_GREEN="\033[0;32m"

# Create cache path for md5
if [ ! -d $CachePath ]; then
    mkdir $CachePath
fi

# Create obj path
if [ ! -d $ObjPath ]; then
    mkdir $ObjPath
fi


# File builder

function compile_file {
    FileName=`basename $1`
    CacheFile=$CachePath/$FileName

    # get current cache
    CurrentCache=`md5sum $1 | awk '{ print $1 }'`

    # get last cache
    if [ -f $CacheFile ]
    then
        LastCache=`cat $CacheFile`
    else
        LastCache=""
    fi


    if [ "$LastCache" != "$CurrentCache" ]
    then
        echo -e "File $COLOR_RED$1$COLOR_DEFAULT is changed"
        Exec=${CPP/SOURCE_FILE/$1}
        ObjFile=${FileName/.cpp/.o}
        Exec=${Exec/OBJECT_FILE/$ObjFile}
        $Exec && echo $CurrentCache > $CacheFile
    else
        echo -e "File $COLOR_GREEN$1$COLOR_DEFAULT is not changed"
    fi
}

function compile {
    List=`ls $1`
    for File in $List
    do
        compile_file $File
    done
}



echo -e $COLOR_GREEN
echo =================================================================
echo 'Compiling'
echo =================================================================
echo -e $COLOR_DEFAULT

compile "hello.cpp"
compile "lib/*.cpp"
compile "lib/neuron/*.cpp"
compile "app/*.cpp"

echo -e $COLOR_GREEN
echo =================================================================
echo 'Make'
echo =================================================================
echo -e $COLOR_DEFAULT

g++ \
-O3 \
-o \
hello \
obj/*.o \
-lGL -lGLU -lX11 -lXrandr -lpthread -lm -lglfw && \
upx -9 hello && \
./hello
