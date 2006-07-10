#!/bin/sh

bail_out()
{
	echo 
	echo "  Something went wrong, bailing out!" 
	echo 
	exit 1
}

echo "Running aclocal -I macros"
aclocal -I macros || bail_out

echo "Running libtoolize"
libtoolize --force || bail_out

echo "Running autoheader"
autoheader -f || bail_out

echo "Running automake -a -c"
automake -a -c || bail_out

#autoconf -f
#./configure --enable-maintainer-mode
echo "Running autoconf"
autoconf || bail_out

echo 
echo "  All autostuff worked fine. "
echo 
