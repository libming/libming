#!/bin/sh
aclocal
libtoolize --force
autoheader -f
automake -a -c
autoconf -f
./configure --enable-maintainer-mode

