include ../Makefile.config

CFLAGS += -I.. -I../src
LDFLAGS += -L.. -lming $(LIBS) -Wl,-rpath,..

PAGER ?= less

BINARIES = listswf listfdb makefdb listjpeg swftoperl swftophp \
	listaction raw2adpcm makeswf listmp3

BINARIES += $(EXTRA_BINS)

all: $(BINARIES)

help:
	$(PAGER) README

clean:
	rm -f *.o core $(BINARIES)

../libming.so:
	cd ..; $(MAKE) dynamic

../libming.a:
	cd ..; $(MAKE) static

listswf_d: blocktypes.o decompile.o decompile5.o read.o listswf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o decompile.o decompile5.o read.o -o listswf 

decompile.o: action.h

listswf: blocktypes.o decompile.o read.o listswf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf 

listaction: blocktypes.o read.o listswf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o read.o -o listaction 

swftoperl: blocktypes.o decompile.o decompile5.o read.o swftoscript.c ../libming.so
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o decompile5.o read.o -o swftoperl 

swftophp: blocktypes.o decompile.o decompile5.o read.o swftoscript.c ../libming.so
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o decompile5.o read.o -o swftophp 

swftopython: 
	echo " swftopython: still not written"

makefdb: blocktypes.o makefdb.c read.o ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) makefdb.c read.o blocktypes.o -o makefdb

gif2mask: gif2mask.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) gif2mask.c -o gif2mask 

png2dbl: png2dbl.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) png2dbl.c -o png2dbl

gif2dbl: gif2dbl.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) gif2dbl.c -o gif2dbl 

raw2adpcm: raw2adpcm.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) raw2adpcm.c -o raw2adpcm 

png2swf: png2swf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) png2swf.c -o png2swf 

makeswf: makeswf.c makeswf.h ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< 

install: $(BINARIES)
	install -d $(bindir)
	install -m 0755 $(BINARIES) $(bindir)
