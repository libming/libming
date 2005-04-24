include ../Makefile.config

CFLAGS += -I.. -I../src 
LDFLAGS += -L.. -lming $(LIBS)

PAGER ?= less

BINARIES = listswf2_d swftoperl2 swftophp2 swftopython makeswf

OLDBINARIES = listswf listfdb makefdb listjpeg swftoperl swftophp \
	listaction raw2adpcm makeswf listmp3 

BINARIES += $(EXTRA_BINS)

all: $(BINARIES)

help:
	$(PAGER) README

clean:
	rm -f *.o core $(BINARIES)

../libming$(NOVAR_SHLIBEXT):
	cd ..; $(MAKE) dynamic

../libming.a:
	cd ..; $(MAKE) static

listswf2_d: main.o blocktypes.o decompile2.o read.o parser.o outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) main.o blocktypes.o parser.o decompile2.o read.o outputtxt.c -o listswf2_d

listswf2: main.o blocktypes.o decompile2.o read.o parser.o outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE main.o blocktypes.o parser.o decompile2.o read.o outputtxt.c -o listswf2

swftoperl2: main.o blocktypes.o decompile2.o read.o parser.o outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) outputscript.c main.o blocktypes.o parser.o decompile2.o read.o -o swftoperl2

swftophp2: main.o blocktypes.o decompile2.o read.o parser.o outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPHP $(CFLAGS) $(LDFLAGS) outputscript.c main.o blocktypes.o parser.o decompile2.o read.o -o swftophp2

swftopython: main.o blocktypes.o decompile2.o read.o parser.o outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPYTHON $(CFLAGS) $(LDFLAGS) outputscript.c main.o blocktypes.o parser.o decompile2.o read.o -o swftopython

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

decompile2.o: decompile.c
	$(CC) $(CFLAGS) -c decompile.c -o decompile2.o

makefdb: blocktypes.o makefdb.c read.o ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) makefdb.c read.o -o makefdb

gif2mask: gif2mask.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) gif2mask.c -o gif2mask 

png2dbl: png2dbl.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) png2dbl.c -o png2dbl

gif2dbl: gif2dbl.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) gif2dbl.c -o gif2dbl 

raw2adpcm: raw2adpcm.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) raw2adpcm.c -o raw2adpcm 

png2swf: png2swf.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) png2swf.c -o png2swf 

makeswf: makeswf.c makeswf.h ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< 

install: $(BINARIES)
	install -d $(bindir)
	install -m 0755 $(BINARIES) $(bindir)
