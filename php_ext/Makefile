include ../Makefile.config

DEFINES = -DHAVE_MING -DCOMPILE_DL_MING -DZEND_NEW_ERROR_HANDLING
CFLAGS = -I../src

php_ming.so: ming.o ../libming.a
	${CC} ${CFLAGS} -shared -fPIC ming.o ../libming.a -o php_ming.so ${EXTRA_LIBS}

ming.o: ming.c
	${CC} ${CFLAGS} -fPIC ${DEFINES} -c ming.c -I. -I../src -I.. `php-config --includes`

../libming.a:
	cd ..; $(MAKE) static

clean:
	rm -f php_ming.so ming.o

install: php_ming.so
	install -d `php-config --extension-dir`
	install php_ming.so `php-config --extension-dir`
	echo "Don't forget to add 'extension=php_ming.so' to php.ini!"
