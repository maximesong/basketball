#!/bin/sh

if [[ `basename $PWD` = "tools" ]]
then  cd ..
fi
ROOT_PATH=$PWD
RELEASE_PATH=$PWD/release-`date +%y-%m-%d`/

rm -rf $RELEASE_PATH
mkdir $RELEASE_PATH
mkdir $RELEASE_PATH/doc
mkdir $RELEASE_PATH/build

cd $ROOT_PATH
qmake
make
cp basketball $RELEASE_PATH

cd $RELEASE_PATH/build && xelatex $ROOT_PATH/doc/doc.tex
cp $RELEASE_PATH/build/doc.pdf $ROOT_PATH/doc/doc.tex $RELEASE_PATH/doc

cp -r $ROOT_PATH/{src,texture,music,lib,tools,dll} $RELEASE_PATH

if [[ -e $ROOT_PATH/basketball.exe ]]; then
    cp $ROOT_PATH/dll/* $RELEASE_PATH
    cp $ROOT_PATH/basketball.exe $RELEASE_PATH
fi

rm -r $RELEASE_PATH/build

cd $RELEASE_PATH
find -name "*~" | xargs rm
zip -r release-`date +%y-%m-%d`.zip .
