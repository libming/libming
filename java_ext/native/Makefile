
JAVADIR=/usr/java/sdk
MINGDIR=../../src

all: libjswf.so

SWFNative.o: SWFNative.cc SWFNative.h
	g++ -Wall -fpic -c -I. -I$(JAVADIR)/include -I$(JAVADIR)/include/linux -I$(MINGDIR) SWFNative.cc

libjswf.so: SWFNative.o
	g++ -Wall -fpic -shared SWFNative.o -L$(MINGDIR) -lming -o $@

clean:
	rm SWFNative.o libjswf.so
