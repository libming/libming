include Rules.make

LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

DIRS = src util py_ext php_ext

all: dynamic #static

install: install-common install-dynamic #install-static

install-common:
	install -d $(LIBDIR)
	install -d $(INCLUDEDIR)
	install -c -m 0644 src/ming.h $(INCLUDEDIR)
	install -c -m 0644 src/ming_config.h $(INCLUDEDIR)
	install -c -m 0644 mingpp.h $(INCLUDEDIR)

install-dynamic: dynamic
	install -c -m 0644 $(SHAREDLIB) $(LIBDIR)/
	(cd $(LIBDIR) && ln -fs $(SHAREDLIB) libming.so.0 && ln -fs libming.so.0 libming.so)

install-static: static
	install -c -m 0644 $(SHAREDLIB) $(LIBDIR)/

dynamic static:
	(cd src && $(MAKE) $@)

clean:
	for i in $(DIRS); do (cd $$i && $(MAKE) clean); done
	if [ -f perl_ext/Makefile ] ; then cd perl_ext; $(MAKE) clean; fi
	rm -f test.o test test.exe *.core *~
	rm -f $(SHAREDLIB) $(SHAREDLIB)
	rm -f libming.a libming.so

distclean: clean
	rm -Rf Rules.make config.make autom4te.cache config.log config.status
