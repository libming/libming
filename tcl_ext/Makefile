

all:swig mingc.so

mingc.so: ming_wrap.o
	gcc -g -Wall -shared -o mingc.so ming_wrap.o -lming

ming_wrap.o: ming_wrap.c
	gcc -g -Wall -I ..  -fpic -c ming_wrap.c

../libming.a:
	cd ..; $(MAKE) static

swig:
	swig -I.. -tcl ming.i

clean:
	rm -f mingc.so core ming_wrap.c ming_wrap.doc *~ *.o 

