include ../config.make

INCLUDES	+= -I..

all: test

test:
	$(CXX) $(CFLAGS) $(LDFLAGS) test.cpp ../libming.a -o test $(LIBS)

clean:
	rm -f test.o test

## Needs to be at the end so variable expand correctly
include ../Rules.make
