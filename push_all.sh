#!/bin/bash
# Repsitory update on the current branch

cd ../..

current=`pwd`;
echo -e "\033[0;32m$current \033[0m"

[ -z "$1" ] && read -p "Enter commit comment: " comment || comment=$1
[ -z "$comment" ] && comment="Auto commit"


# Libraries

cd $current && cd ./lib/core && ./push "$comment" && \
cd $current && cd ./lib/graph && ./push "$comment" && \
cd $current && cd ./lib/json && ./push "$comment" && \
cd $current && cd ./lib/sock && ./push "$comment" && \

cd $current && cd ./app/shab && ./push "$comment" && \
cd $current && cd ./app/shoggoth && ./push "$comment" && \

echo COMPLETE
