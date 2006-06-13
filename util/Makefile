include ../Makefile.config

CFLAGS += -I.. -I../src 
LDFLAGS += -L.. -lming $(LIBS)

UTILOBJ = action.o blocktypes.o decompile.o main.o parser.o read.o

PAGER ?= less

BINARIES = listswf listswf_d swftoperl swftophp swftopython \
	makeswf listaction listaction_d listfdb makefdb makefdb2 listjpeg raw2adpcm \
	listmp3 

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

listswf_d: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) outputtxt.c $(UTILOBJ) $(LDFLAGS) -o listswf_d

listswf: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) -DNODECOMPILE outputtxt.c $(UTILOBJ) $(LDFLAGS) -o listswf

listaction_d: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) -DACTIONONLY outputtxt.c $(UTILOBJ) $(LDFLAGS) -o listaction_d

listaction: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) -DACTIONONLY -DNODECOMPILE  outputtxt.c $(UTILOBJ) $(LDFLAGS) -o listaction

swftoperl: $(UTILOBJ) outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPERL $(CFLAGS) outputscript.c $(UTILOBJ) $(LDFLAGS) -o swftoperl

swftophp: $(UTILOBJ) outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPHP $(CFLAGS) outputscript.c $(UTILOBJ) $(LDFLAGS) -o swftophp

swftopython: $(UTILOBJ) outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPYTHON $(CFLAGS) outputscript.c $(UTILOBJ) $(LDFLAGS) -o swftopython

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

decompile.o: decompile.c
	$(CC) $(CFLAGS) -c decompile.c -o decompile.o

makefdb2: outputfdb.c $(UTILOBJ) ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $< $(UTILOBJ) $(LDFLAGS) -o $@

makefdb: blocktypes.o makefdb.c read.o ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) makefdb.c $(LDFLAGS) read.o -o makefdb

gif2mask: gif2mask.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) gif2mask.c $(LDFLAGS) -o gif2mask 

png2dbl: png2dbl.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) png2dbl.c $(LDFLAGS) -o png2dbl

dbl2png: dbl2png.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) dbl2png.c $(LDFLAGS) -o dbl2png

gif2dbl: gif2dbl.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) gif2dbl.c $(LDFLAGS) -o gif2dbl 

raw2adpcm: raw2adpcm.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) raw2adpcm.c $(LDFLAGS) -o raw2adpcm 

png2swf: png2swf.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) png2swf.c $(LDFLAGS) -o png2swf 

makeswf: makeswf.c makeswf.h ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< 

install: $(BINARIES)
	$(INSTALL) -d $(bindir)
	$(INSTALL_PROGRAM) -m 0755 $(BINARIES) $(bindir)
	$(INSTALL_PROGRAM) -m 0755 ming-config $(bindir)
