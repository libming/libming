PYVERSION = 2.3
PYPREFIX = /usr
PYINCDIR = $(PYPREFIX)/include/python$(PYVERSION)
PYLIBDIR = $(PYPREFIX)/lib/python$(PYVERSION)

all: _mingcmodule.so

_mingcmodule.so: ming_wrap.o
	gcc -g -Wall -shared -o _mingcmodule.so ming_wrap.o -lming

mingc.pyd: ming_wrap.o
	dllwrap --dllname mingc.pyd --driver-name gcc --def mingc.def \
	  -o mingc.pyd ming_wrap.o -s --entry _DllMain@12 \
	  --target=i386-mingw32 -Llib/ -lpython15

ming_wrap.o: ming_wrap.c
	gcc -g -Wall -I .. -I ${PYINCDIR} -fpic -c ming_wrap.c

ming_wrap.c: swig

../libming.a:
	cd ..; make static

swig:
	swig -I.. -python ming.i

clean:
	rm -f _mingcmodule.so mingc.py *.pyc core *~ *.o ming_wrap.c ming_wrap.doc

install: _mingcmodule.so
	install -d $(DESTDIR)${PYLIBDIR}/site-packages
	install -m 0644 _mingcmodule.so $(DESTDIR)${PYLIBDIR}/site-packages
	install -m 0644 ming.py $(DESTDIR)${PYLIBDIR}/site-packages
	install -m 0644 mingc.py $(DESTDIR)$(PYLIBDIR)/site-packages

