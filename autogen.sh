#!/bin/sh
aclocal
libtoolize --force
automake -a
autoconf
