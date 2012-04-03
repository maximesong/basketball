#!/bin/sh

WINDOWS_PATH=/home/song/Windows

if [[ `basename $PWD` = "tools" ]]
then  cd ..
fi
ROOT_PATH=$PWD


cp $WINDOWS_PATH/basketball.exe $ROOT_PATH