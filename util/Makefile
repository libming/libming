include ../config.make
include ../Rules.make

INCLUDES += -I.. -I../src

PAGER ?= less

BINARIES = listswf listfdb makefdb listjpeg swftoperl swftophp \
	listaction raw2adpcm makeswf listmp3

BINARIES += $(EXTRA_BINS)

all: $(BINARIES)

help:
	$(PAGER) README

clean:
	rm -f *.o core $(BINARIES)

listswf_d: blocktypes.o decompile.o decompile5.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o decompile.o decompile5.o read.o -o listswf $(MATHLIB) $(ZLIB)

decompile.o: action.h

listswf: blocktypes.o decompile.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf $(MATHLIB) $(ZLIB)

listaction: blocktypes.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o read.o -o listaction -lm

swftoperl: blocktypes.o decompile.o decompile5.o read.o swftoscript.c
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o decompile5.o read.o -o swftoperl $(MATHLIB) $(ZLIB)

swftophp: blocktypes.o decompile.o decompile5.o read.o swftoscript.c
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o decompile5.o read.o -o swftophp $(MATHLIB) $(ZLIB)

swftopython: 
	echo " swftopython: still not written"

makefdb: blocktypes.o makefdb.c read.o
	$(CC) $(CFLAGS) $(LDFLAGS) makefdb.c read.o blocktypes.o -o makefdb

gif2mask: gif2mask.c
	$(CC) $(CFLAGS) $(LDFLAGS) gif2mask.c -o gif2mask $(GIFLIB) $(MATHLIB) $(ZLIB)

png2dbl: png2dbl.c
	$(CC) $(CFLAGS) $(LDFLAGS) png2dbl.c -o png2dbl $(ZLIB) $(PNGLIB) $(MATHLIB)

gif2dbl: gif2dbl.c
	$(CC) $(CFLAGS) $(LDFLAGS) gif2dbl.c -o gif2dbl $(ZLIB) $(GIFLIB) $(MATHLIB)

../libming.a:
	cd ..; $(MAKE) static

raw2adpcm: raw2adpcm.c ../libming.a
	$(CC) $(CFLAGS) $(LDFLAGS) raw2adpcm.c -o raw2adpcm ../libming.a $(LIBS)

png2swf: png2swf.c
	$(CC) $(CFLAGS) $(LDFLAGS) png2swf.c -o png2swf ../libming.a $(LIBS)

makeswf: makeswf.c makeswf.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -L.. -lming $(MATHLIB) $(ZLIB)

install: $(BINARIES)
	install -d $(bindir)
	install -m 0755 $(BINARIES) $(bindir)
