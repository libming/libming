
JAVADIR=/usr/java
MINGDIR=../..

all: libjswf.so

SWFNative.o: SWFNative.cc SWFNative.h
	gcc -Wall -fpic -c -I. -I$(JAVADIR)/include -I$(JAVADIR)/include/linux -I$(MINGDIR) SWFNative.cc

libjswf.so: SWFNative.o
	gcc -Wall -fpic -shared SWFNative.o -L$(MINGDIR) -lming -o $@

