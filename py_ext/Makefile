#PYINCDIR = /usr/local/include/python2.0
#PYLIBDIR = /usr/local/lib/python2.0

PYINCDIR = /usr/include/python1.5
PYLIBDIR = /usr/lib/python1.5

all:

mingcmodule.so: ming_wrap.o ../libming.a
	gcc -g -Wall -shared -o mingcmodule.so ming_wrap.o ../libming.a

mingc.pyd: ming_wrap.o
	dllwrap --dllname mingc.pyd --driver-name gcc --def mingc.def \
	  -o mingc.pyd ming_wrap.o -s --entry _DllMain@12 \
	  --target=i386-mingw32 -Llib/ -lpython15

ming_wrap.o: ming_wrap.c
	gcc -g -Wall -I .. -I ${PYINCDIR} -fpic -c ming_wrap.c

../libming.a:
	cd ..; make static

swig:
	swig -I.. -python ming.i

clean:
	rm -f mingcmodule.so core *~ *.o ming.pyc

install:
	cp mingcmodule.so ming.py ${PYLIBDIR}/site-packages
