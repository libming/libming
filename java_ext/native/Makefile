
CXXFLAGS += -I../.. -I../../src -I.
LDFLAGS += -L../.. -lming $(LIBS)

JAVADIR=/usr

all: libjswf$(NOVAR_SHLIBEXT)

SWFNative.o: SWFNative.cc SWFNative.h
	$(CXX) $(CXXFLAGS) -fpic -c -I$(JAVADIR)/include -I$(JAVADIR)/include/linux  SWFNative.cc -o $@

libjswf$(NOVAR_SHLIBEXT): SWFNative.o
	$(CXX) $(CXXFLAGS) -fpic -shared SWFNative.o $(LDFLAGS) -o $@

clean:
	rm SWFNative.o libjswf$(NOVAR_SHLIBEXT)
