include ../config.make
include ../Rules.make

INCLUDES	+= -I.. 

PAGER ?= less

all: listswf listfdb makefdb listjpeg swftoperl swftophp \
	png2dbl listaction raw2adpcm png2swf makeswf listmp3

help:
	$(PAGER) README

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
		  png2swf png2swf.exe \
		  gif2dbl gif2dbl.exe \
		  raw2adpcm raw2adpcm.exe \
		  gif2mask gif2mask.exe makeswf makeswf.exe core

#.o: .c .h

listswf_d: blocktypes.o decompile.o decompile5.o read.o listswf.c
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o decompile.o decompile5.o read.o -o listswf -lm -lz

decompile.o: action.h

listswf: blocktypes.o decompile.o read.o listswf.c
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

install: all
	install -d $(bindir)
	install -m 0755 swftophp $(bindir)
	install -m 0755 swftoperl $(bindir)
	install -m 0755 makefdb $(bindir)
	install -m 0755 listfdb $(bindir)
	install -m 0755 listjpeg $(bindir)
	install -m 0755 listswf $(bindir)
	install -m 0755 listaction $(bindir)
	install -m 0755 png2dbl $(bindir)
	#install -m 0755 gif2dbl $(bindir)
	#install -m 0755 gif2mask $(bindir)
	install -m 0755 raw2adpcm $(bindir)
	install -m 0755 png2swf $(bindir)
	install -m 0755 makeswf $(bindir)
