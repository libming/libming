#!/bin/sh
mkdir tmp
cp * tmp
cd tmp
phpize
./configure --with-ming
make
#make install
