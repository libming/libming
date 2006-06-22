#!/bin/sh

echo "Running aclocal"
aclocal

echo "Running libtoolize"
libtoolize --force

echo "Running autoheader"
autoheader -f

echo "Running automake -a -c"
automake -a -c

#autoconf -f
#./configure --enable-maintainer-mode
echo "Running autoconf"
autoconf

