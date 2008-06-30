#!/bin/sh

if [  z"$1" = z ]; then
	srcdir=.
else
	srcdir=$1
fi

mkdir -p tmp/modules
cp $srcdir/* tmp
cp `php-config --extension-dir`/*.so tmp/modules 
cd tmp
phpize
./configure --with-ming
make
#make install
