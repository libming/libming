#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

PREFIX = ${DESTDIR}/usr
# Default binary location for install: /usr/lib/libming/bin
BINDIR = ${PREFIX}/lib/libming/bin

all:
	cat README

all_binaries: listswf listfdb makefdb listjpeg swftoperl swftophp png2dbl gif2dbl \
	gif2mask listaction raw2adpcm png2swf

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
		  gif2mask gif2mask.exe core

#.o: .c .h

listswf: blocktypes.o decompile.o read.o listswf.c
	${CC} listswf.c blocktypes.o decompile.o read.o -o listswf -lm -lz

decompile.o: action.h

listswf_nd: blocktypes.o decompile.o read.o listswf.c
	${CC} -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf -lm -lz

listaction: blocktypes.o read.o listswf.c
	${CC} -DNODECOMPILE listswf.c blocktypes.o read.o -o listaction -lm

swftoperl: blocktypes.o decompile.o decompile5.o read.o swftoperl.c
	${CC} swftoperl.c blocktypes.o decompile.o decompile5.o read.o -o swftoperl -lm

swftophp: blocktypes.o decompile.o decompile5.o read.o swftophp.c
	${CC} swftophp.c blocktypes.o decompile.o decompile5.o read.o -o swftophp -lm

swftopython: 
	echo " swftopython: still not written"

makefdb: blocktypes.o makefdb.c read.o
	${CC} makefdb.c read.o blocktypes.o -o makefdb

gif2mask: gif2mask.c
	${CC} gif2mask.c -o gif2mask -lungif -lm -lz

png2dbl: png2dbl.c
	${CC} png2dbl.c -o png2dbl -lz -lpng -lm

gif2dbl: gif2dbl.c
	${CC} gif2dbl.c -o gif2dbl -lz -lungif -lm

raw2adpcm: raw2adpcm.c
	${CC} raw2adpcm.c -o raw2adpcm -lming -lm

png2swf: png2swf.c
	${CC} png2swf.c -o png2swf ../ming/libming.a -lm -lz -lpng

install: all_binaries
	install -d $(BINDIR)
	install -m 0755 swftophp $(BINDIR)
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
