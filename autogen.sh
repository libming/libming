#!/bin/sh

bail_out()
{
	echo 
	echo "  Something went wrong, bailing out!" 
	echo 
	exit 1
}

LIBTOOLIZE=libtoolize
command -v $LIBTOOLIZE >/dev/null 2>&1 || LIBTOOLIZE=glibtoolize

echo "Running $LIBTOOLIZE -f -c"
$LIBTOOLIZE -f -c || bail_out

echo "Running aclocal -I macros"
aclocal -I macros || bail_out

echo "Running $LIBTOOLIZE --automake"
$LIBTOOLIZE --automake || bail_out

echo "Running autoheader -f"
autoheader -f || bail_out

echo "Running automake"
automake --add-missing -c  > /dev/null || bail_out

echo "Running autoconf"
autoconf || bail_out

echo 
echo "  All autostuff worked fine. "
echo 
