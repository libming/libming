include ../Makefile.config

CFLAGS += -I.. -I../src
LDFLAGS += -L.. -lming $(LIBS) -Wl,-rpath,..

PAGER ?= less

NEWBINARIES = listswf2 listswf2_d swftoperl2 swftophp2 swftopython

BINARIES = listswf listfdb makefdb listjpeg swftoperl swftophp \
	listaction raw2adpcm makeswf listmp3 $(NEWBINARIES)

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

listswf_d: blocktypes.o decompile.o read.o parser.o listswf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o parser.o decompile.o read.o -o listswf_d

listswf: blocktypes.o decompile.o read.o parser.o listswf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o parser.o decompile.o read.o -o listswf 

listswf2_d: main.o blocktypes.o decompile2.o read.o parser.o outputtxt.o ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) main.o blocktypes.o parser.o decompile2.o read.o outputtxt.o -o listswf2_d

listswf2: main.o blocktypes.o decompile2.o read.o parser.o outputtxt.o ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE main.o blocktypes.o parser.o decompile2.o read.o outputtxt.o -o listswf2

listaction: blocktypes.o read.o parser.o decompile.o listswf.c ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o parser.o decompile.o read.o -o listaction 

swftoperl: blocktypes.o decompile.o read.o parser.o swftoscript.c ../libming.so
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o parser.o decompile.o read.o -o swftoperl 

swftoperl2: main.o blocktypes.o decompile2.o read.o parser.o outputscript.c ../libming.so
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) outputscript.c main.o blocktypes.o parser.o decompile2.o read.o -o swftoperl2

swftophp: blocktypes.o decompile.o read.o parser.o swftoscript.c ../libming.so
	$(CC) -DSWFPHP $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o parser.o decompile.o read.o -o swftophp 

swftophp2: main.o blocktypes.o decompile2.o read.o parser.o swftoscript.c ../libming.so
	$(CC) -DSWFPHP $(CFLAGS) $(LDFLAGS) outputscript.c main.o blocktypes.o parser.o decompile2.o read.o -o swftophp2

swftopython: main.o blocktypes.o decompile2.o read.o parser.o swftoscript.c ../libming.so
	$(CC) -DSWFPYTHON $(CFLAGS) $(LDFLAGS) outputscript.c main.o blocktypes.o parser.o decompile2.o read.o -o swftopython

parser.o: parser.c
	$(CC) -DNEWUTILS $(CFLAGS) -c parser.c

decompile2.o: decompile.c
	$(CC) -DNEWUTILS $(CFLAGS) -c decompile.c -o decompile2.o

makefdb: blocktypes.o makefdb.c read.o ../libming.so
	$(CC) $(CFLAGS) $(LDFLAGS) makefdb.c read.o -o makefdb

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
