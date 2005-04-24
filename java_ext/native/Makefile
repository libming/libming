
JAVADIR=/usr/java/sdk
MINGDIR=../../src

all: libjswf$(NOVAR_SHLIBEXT)

SWFNative.o: SWFNative.cc SWFNative.h
	g++ -Wall -fpic -c -I. -I$(JAVADIR)/include -I$(JAVADIR)/include/linux -I$(MINGDIR) SWFNative.cc

libjswf$(NOVAR_SHLIBEXT): SWFNative.o
	g++ -Wall -fpic -shared SWFNative.o -L$(MINGDIR) -lming -o $@

clean:
	rm SWFNative.o libjswf$(NOVAR_SHLIBEXT)
