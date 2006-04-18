include ../../Makefile.config

CFLAGS += -I.. -I../.. -I../../src
LDFLAGS += -L../.. -lming $(LIBS) -Wl,-rpath,../..

PAGER ?= less

BINARIES = listswf listaction listswf_d 

all: $(BINARIES)

clean:
	rm -f *.o core $(BINARIES)

../../libming$(NOVAR_SHLIBEXT):
	cd ..; $(MAKE) dynamic

../../libming.a:
	cd ..; $(MAKE) static

listswf_d: blocktypes.o ../decompile.o read.o listswf.c ../../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) listswf.c blocktypes.o ../decompile.o read.o -o listswf_d

listswf: blocktypes.o ../decompile.o read.o listswf.c ../../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o ../decompile.o read.o -o listswf 

listaction: blocktypes.o read.o listswf.c ../../libming$(NOVAR_SHLIBEXT)
	$(CC) $(CFLAGS) $(LDFLAGS) -DNODECOMPILE listswf.c blocktypes.o read.o -o listaction 

install: $(BINARIES)
	$(INSTALL) -d $(bindir)
	$(INSTALL_PROGRAM) -m 0755 $(BINARIES) $(bindir)
