include ../config.make
include ../Rules.make

prefix=`php-config --prefix`

# If you don't want to modify this file you can use
# the environmental variable PHPSRC instead.
#ifeq (${PHPSRC},)
#        PHPSRC=/usr/src/php-4.3.0
#endif
PHPSRC ?= /usr/src/php-4.3.0

DEFINES = -DHAVE_MING -DCOMPILE_DL_MING -DZEND_NEW_ERROR_HANDLING
CFLAGS = -I../src
CFLAGS += -I${PHPSRC}
CFLAGS += -I${PHPSRC}/TSRM
CFLAGS += -I${PHPSRC}/main
CFLAGS += -I${PHPSRC}/Zend

CFILES = ming.c

php_ming.so: ming.o ../libming.a
	${CC} ${CFLAGS} -shared -fPIC ming.o ../libming.a -o php_ming.so ${EXTRA_LIBS}

ming.o: ${CFILES}
	${CC} ${CFLAGS} -fPIC ${DEFINES} -c ming.c -I. -I../src -I.. `php-config --includes`

../libming.a:
	cd ..; $(MAKE) static

clean:
	rm -f php_ming.so ming.o

install: php_ming.so
	install -d $(DESTDIR)`php-config --extension-dir`
	install php_ming.so $(DESTDIR)`php-config --extension-dir`
	echo "Don't forget to add 'extension=php_ming.so' to php.ini!"
