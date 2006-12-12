#!/bin/sh

if [  z"$1" = z ]; then
	srcdir=.
else
	srcdir=$1
fi

mkdir tmp
cp $srcdir/* tmp
cd tmp
phpize
./configure --with-ming
make
#make install
