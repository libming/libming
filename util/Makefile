include ../config.make

PREFIX = $(DESTDIR)/usr
# Default binary location for install: /usr/lib/libming/bin
BINDIR = $(PREFIX)/lib/libming/bin

INCLUDES	+= -I.. 

PAGER ?= less

## Needs to be placed after variable defs but before rules
include ../Rules.make

all:
	$(PAGER) README

all_binaries: listswf_nd listfdb makefdb listjpeg swftoperl swftophp \
	png2dbl listaction raw2adpcm png2swf makeswf listmp3

clean:
	rm -f *.o listswf listswf.exe \
		  listaction listaction.exe \
		  bindump bindump.exe \
		  hexdump hexdump.exe \
		  listfdb listfdb.exe \
		  listjpeg listjpeg.exe \
		  listmp3 listmp3.exe \
		  makefdb makefdb.exe \
		  swftoperl swftoperl.exe \
		  swftophp swftophp.exe \
		  png2dbl png2dbl.exe \
		  gif2dbl gif2dbl.exe \
		  raw2adpcm raw2adpcm.exe \
		  gif2mask gif2mask.exe makeswf makeswf.exe core

#.o: .c .h

listswf: blocktypes.o decompile.o decompile5.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o decompile.o decompile5.o read.o -o listswf -lm -lz

decompile.o: action.h

listswf_nd: blocktypes.o decompile.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf -lm -lz

listaction: blocktypes.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o read.o -o listaction -lm

swftoperl: blocktypes.o decompile.o decompile5.o read.o swftoscript.c
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o decompile5.o read.o -o swftoperl -lm -lz

swftophp: blocktypes.o decompile.o decompile5.o read.o swftoscript.c
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) swftoscript.c blocktypes.o decompile.o decompile5.o read.o -o swftophp -lm -lz

swftopython: 
	echo " swftopython: still not written"

makefdb: blocktypes.o makefdb.c read.o
	$(CC) $(CFLAGS) $(LDFLAGS) makefdb.c read.o blocktypes.o -o makefdb

gif2mask: gif2mask.c
	$(CC) $(CFLAGS) $(LDFLAGS) gif2mask.c -o gif2mask -lungif -lm -lz

png2dbl: png2dbl.c
	$(CC) $(CFLAGS) $(LDFLAGS) png2dbl.c -o png2dbl -lz -lpng -lm

gif2dbl: gif2dbl.c
	$(CC) $(CFLAGS) $(LDFLAGS) gif2dbl.c -o gif2dbl -lz -lungif -lm

../libming.a:
	cd ..; $(MAKE) static

raw2adpcm: raw2adpcm.c ../libming.a
	$(CC) $(CFLAGS) $(LDFLAGS) raw2adpcm.c -o raw2adpcm ../libming.a $(LIBS)

png2swf: png2swf.c
	$(CC) $(CFLAGS) $(LDFLAGS) png2swf.c -o png2swf ../libming.a $(LIBS)

makeswf: makeswf.c makeswf.h
	$(CC) $(CFLAGS) $(LDFLAGS) -lming -o $@ $<

install: all_binaries
	install -d $(BINDIR)
	install -m 0755 swftophp $(BINDIR)
	install -m 0755 swftoperl $(BINDIR)
	install -m 0755 makefdb $(BINDIR)
	install -m 0755 listfdb $(BINDIR)
	install -m 0755 listjpeg $(BINDIR)
	install -m 0755 listswf $(BINDIR)
	install -m 0755 listaction $(BINDIR)
	install -m 0755 png2dbl $(BINDIR)
	install -m 0755 gif2dbl $(BINDIR)
	install -m 0755 gif2mask $(BINDIR)
	install -m 0755 raw2adpcm $(BINDIR)
	install -m 0755 png2swf $(BINDIR)
