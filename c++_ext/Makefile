include ../Makefile.config

CFLAGS += -I..

all: test

test:
	$(CXX) $(CFLAGS) $(LDFLAGS) test.cpp ../libming.a -o test $(LIBS)

clean:
	rm -f test.o test
