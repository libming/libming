#!/bin/sh

echo "Running aclocal"
aclocal || exit 1

echo "Running libtoolize"
libtoolize --force || exit 1

echo "Running autoheader"
autoheader -f || exit 1

echo "Running automake -a -c"
automake -a -c || exit 1

#autoconf -f
#./configure --enable-maintainer-mode
echo "Running autoconf"
autoconf || exit 1

echo 
echo "  All autostuff worked fine. "
echo 
