
prefix=`php-config --prefix`

DEFINES = -DHAVE_MING -DCOMPILE_DL_MING -DZEND_NEW_ERROR_HANDLING

CFILES = ming.c swfaction.c swfbitmap.c swfbutton.c swfdisplayitem.c \
	 swffill.c swffont.c swfgradient.c swfmorph.c swfmovie.c \
	 swfshape.c swfsprite.c swftext.c swftextfield.c

php_ming.so: ming.o
	${CC} -shared -fpic ming.o ../libming.a -o php_ming.so -lm

ming.o: ${CFILES}
	${CC} ${DEFINES} -c ming.c \
	  -I. -I.. -I${prefix}/include/php -I${prefix}/include/php/main \
	  -I${prefix}/include/php/Zend -I${prefix}/include/php/TSRM -lm

clean:
	rm -f php_ming.so

install:
	cp php_ming.so `php-config --extension-dir`
	echo "Don't forget to add 'extension=php_ming.so' to php.ini!"
