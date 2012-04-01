#!/bin/sh

if [[ `basename $PWD` = "tools" ]]
then  cd ..
fi
ROOT_PATH=$PWD
RELEASE_PATH=$PWD/release/

rm -r $RELEASE_PATH
mkdir $RELEASE_PATH
mkdir $RELEASE_PATH/doc
mkdir $RELEASE_PATH/build

cd $ROOT_PATH
qmake
make
cp basketball $RELEASE_PATH

cd $RELEASE_PATH/build && xelatex $ROOT_PATH/doc/doc.tex
cp $RELEASE_PATH/build/doc.pdf $RELEASE_PATH/doc

cp -r $ROOT_PATH/{src,texture,music,lib,tools} $RELEASE_PATH

rm -r $RELEASE_PATH/build

cd $RELEASE_PATH
find -name "*~" | xargs rm
zip -r release-`date +%y-%m-%d`.zip .
