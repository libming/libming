include ../Makefile.config

DEFINES = -DHAVE_MING -DCOMPILE_DL_MING -DZEND_NEW_ERROR_HANDLING
CFLAGS = -I../src

php_ming$(NOVAR_SHLIBEXT): ming.o
	${CC} ${CFLAGS} -shared -fPIC ming.o -L.. -lming -o php_ming$(NOVAR_SHLIBEXT) ${EXTRA_LIBS}

ming.o: ming.c
	${CC} ${CFLAGS} -fPIC ${DEFINES} -c ming.c -I. -I../src -I.. `php-config --includes`

clean:
	rm -f php_ming$(NOVAR_SHLIBEXT) ming.o

install: php_ming$(NOVAR_SHLIBEXT)
	$(INSTALL) -d $(DESTDIR)`php-config --extension-dir`
	$(INSTALL_PROGRAM) php_ming$(NOVAR_SHLIBEXT) $(DESTDIR)`php-config --extension-dir`
	echo "Don't forget to add 'extension=php_ming$(NOVAR_SHLIBEXT)' to php.ini!"
