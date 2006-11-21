Oct 2006: This module has been resynced with the PHP5 code, and modified
to build for PHP4 also. Just use the appropriate version of phpize.

Feb 2006: This ming.c will work with PHP4, and not PHP5 or higher. It
is provided here for those that are not able to upgrade to PHP5 yet.

- Stuart



You can use the Makefile here to try and build ming as a php module- just
run "make" and then "make install".  And as the make install command tells
you, you'll have to add "extension=php_ming.so" to your php.ini file, or
you can just put "dl('php_ming.so');" at the beginning of all your ming
scripts- though I hear that doesn't work on solaris..

Or you can try using the make_module.sh script to use php's "phpize"
script to install Ming as a php module.  Unless you're sure what each of
the commands in the make_module.sh script will do, and how any will deal
with previous errors, I'd suggest running them on the command line
yourself.  I haven't even tested it myself, because I don't know what they
do and since I've already got Ming built into my php, I don't know if a
conflict will kill php and how hard it will be to clean up the mess.  Also
note that the autoconf nonsense that this does will kill the existing
Makefile.

So there's my disclaimer, now have fun.

_dave
