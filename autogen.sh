#!/bin/sh
aclocal
libtoolize --force
automake -a
autoconf
./configure --enable-maintainer-mode

