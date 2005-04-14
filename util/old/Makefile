include ../../Makefile.config

CFLAGS += -I.. -I../.. -I../../src
LDFLAGS += -L../.. -lming $(LIBS) -Wl,-rpath,../..

PAGER ?= less

BINARIES = listswf listaction listswf_d swftoperl swftophp 

all: $(BINARIES)

clean:
	rm -f *.o core $(BINARIES)

../../libming.so:
	cd ..; $(MAKE) dynamic

../../libming.a:
	cd ..; $(MAKE) static

listswf_d: blocktypes.o decompile.o read.o listswf.c ../../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o decompile.o read.o -o listswf_d

listswf: blocktypes.o decompile.o read.o listswf.c ../../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf 

listaction: blocktypes.o read.o listswf.c ../../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o read.o -o listaction 

swftoperl: blocktypes.o decompile.o read.o swftoscript.c ../../libming.so
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o read.o -o swftoperl 

swftophp: blocktypes.o decompile.o read.o swftoscript.c ../../libming.so
	$(CC) -DSWFPHP $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o read.o -o swftophp 

swftopython: 
	echo " swftopython: still not written"

install: $(BINARIES)
	install -d $(bindir)
	install -m 0755 $(BINARIES) $(bindir)
