#!/bin/bash

# Execute
Execute="true"

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



function info {
    if [ $2 = "CHANGE" ]
    then
        echo -e "File $COLOR_RED$1$COLOR_DEFAULT is changed"
    fi
    if [ $2 = "NO_CHANGE" ]
    then
        echo -e "File $COLOR_GREEN$1$COLOR_DEFAULT is not changed"
    fi
}


function checkControlSumm {
    if [ -f $1 ]
    then
        FileName=`basename $1`
        CacheFile=$CachePath/$FileName

        CurrentCache=`md5sum $1 | awk '{ print $1 }'`

        # get last cache
        if [ -f $CacheFile ]
        then
            LastCache=`cat $CacheFile`
        else
            LastCache="NO_CACHE"
        fi

        if [ "$LastCache" != "$CurrentCache" ]
        then
            echo "CHANGE"
        else
            echo "NO_CHANGE"
        fi
    else
        echo "NO_CHANGE"
    fi
}



function storeControlSumm {
    # Get cache file name
    FileName=`basename $1`
    CacheFile=$CachePath/$FileName

    # GetCurrent cache
    CurrentCache=`md5sum $1 | awk '{ print $1 }'`

    echo $CurrentCache > $CacheFile
}


# Compile one cpp file
function compile_file {
    FileNameSource=$1
    FileNameHeader=${FileNameSource/.cpp/.h}
    RebuildSource=$( checkControlSumm $FileNameSource )
    RebuildHeader=$( checkControlSumm $FileNameHeader )

    info $FileNameSource $RebuildSource
    info $FileNameHeader $RebuildHeader

    if [[ $RebuildSource = "CHANGE" || $RebuildHeader = "CHANGE" ]]
    then
        FileName=`basename $1`
        Exec=${CPP/SOURCE_FILE/$1}
        ObjFile=${FileName/.cpp/.o}
        Exec=${Exec/OBJECT_FILE/$ObjFile}
        echo -e "Run compiler:"$COLOR_GREEN $Exec $COLOR_DEFAULT
        $Exec
        if [ $? -eq 0 ]
        then
            storeControlSumm $FileNameSource
            storeControlSumm $FileNameHeader
            echo -e $COLOR_GREEN"ok"$COLOR_DEFAULT
        else
            Execute="false"
        fi
    fi
}


# Compile path
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



if [ $Execute = "true" ]
then
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

fi
