include ../config.make

prefix=`php-config --prefix`

DEFINES = -DHAVE_MING -DCOMPILE_DL_MING -DZEND_NEW_ERROR_HANDLING

CFILES = ming.c

php_ming.so: ming.o ../libming.a
	${CC} -shared -fPIC ming.o ../libming.a -o php_ming.so ${EXTRA_LIBS}

ming.o: ${CFILES}
	${CC} -fPIC ${DEFINES} -c ming.c -I. -I.. -I../src `php-config --includes`

../libming.a:
	cd ..; make static

clean:
	rm -f php_ming.so ming.o

install: php_ming.so
	install -d $(DESTDIR)`php-config --extension-dir`
	install php_ming.so $(DESTDIR)`php-config --extension-dir`
	echo "Don't forget to add 'extension=php_ming.so' to php.ini!"
