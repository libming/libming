include ../Makefile.config

CFLAGS += -I.. -I../src 
LDFLAGS += -L.. -lming $(LIBS)

UTILOBJ = action.o blocktypes.o decompile.o main.o parser.o read.o

PAGER ?= less

BINARIES = listswf listswf_d swftoperl swftophp swftopython makeswf \
	listaction listaction_d listfdb makefdb listjpeg raw2adpcm listmp3

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
	$(CC) $(CFLAGS) $(LDFLAGS) $(UTILOBJ) outputtxt.c -o listswf_d

listswf: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE $(UTILOBJ) outputtxt.c -o listswf

listaction_d: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) -DACTIONONLY $(LDFLAGS) $(UTILOBJ) outputtxt.c -o listaction_d

listaction: $(UTILOBJ) outputtxt.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) -DACTIONONLY $(LDFLAGS) -DNODECOMPILE $(UTILOBJ) outputtxt.c -o listaction

swftoperl: $(UTILOBJ) outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPERL $(CFLAGS) $(LDFLAGS) outputscript.c $(UTILOBJ) -o swftoperl

swftophp: $(UTILOBJ) outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPHP $(CFLAGS) $(LDFLAGS) outputscript.c $(UTILOBJ) -o swftophp

swftopython: $(UTILOBJ) outputscript.c ../libming$(NOVAR_SHLIBEXT)
	$(CC) -DSWFPYTHON $(CFLAGS) $(LDFLAGS) outputscript.c $(UTILOBJ) -o swftopython

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

decompile.o: decompile.c
	$(CC) $(CFLAGS) -c decompile.c -o decompile.o

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
