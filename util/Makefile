#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

all:
	cat README

clean:
	rm -f *.o listswf listswf.exe \
		  bindump bindump.exe \
		  hexdump hexdump.exe \
		  listfdb listfdb.exe \
		  listjpeg listjpeg.exe \
		  listmp3 listmp3.exe \
		  makefdb makefdb.exe \
		  swftophp swftophp.exe \
		  png2dbl png2dbl.exe \
		  gif2dbl gif2dbl.exe \
		  gif2mask gif2mask.exe core

#.o: .c .h

listswf: blocktypes.o decompile.o read.o listswf.c
	${CC} listswf.c blocktypes.o decompile.o read.o -o listswf -lm -lz

decompile.o: action.h

listswf_nd: blocktypes.o decompile.o read.o listswf.c
	${CC} -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf -lm -lz

listaction: blocktypes.o decompile.o read.o listswf.c
	${CC} -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf -lm

swftophp: blocktypes.o decompile.o read.o swftophp.c
	${CC} swftophp.c blocktypes.o decompile.o read.o -o swftophp -lm

makefdb: blocktypes.o makefdb.c read.o
	${CC} makefdb.c read.o blocktypes.o -o makefdb

gif2mask: gif2mask.c
	${CC} gif2mask.c -o gif2mask -lgif -lm -lz

png2dbl: png2dbl.c
	${CC} png2dbl.c -o png2dbl -lz -lpng -lm

gif2dbl: gif2dbl.c
	${CC} gif2dbl.c -o gif2dbl -lz -lungif -lm

