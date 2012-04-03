#!/bin/sh

WINDOWS_PATH=/home/song/Windows

if [[ `basename $PWD` = "tools" ]]
then  cd ..
fi
ROOT_PATH=$PWD
RELEASE_PATH=$PWD/release-`date +%y-%m-%d`/

cp -r $RELEASE_PATH $WINDOWS_PATH