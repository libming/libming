
DEFS = @DEFS@ -DNODECOMPILE

if HAVE_GIF
if HAVE_ZLIB
gifprograms = gif2mask gif2dbl
else
gifprograms =
endif
else
gifprograms =
endif

if HAVE_PNG
pngprograms = png2dbl
else
pngprograms =
endif

EXTRA_PROGRAMS = \
	listswf \
	swftophp \
	makefdb \
	$(pngprograms) \
	$(gifprograms)

noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = \
	blocktypes.c \
	decompile.c \
	read.c 

libutil_la_LDFLAGS = $(MATHLIB)

noinst_HEADERS = \
	action.h \
	blocktypes.h \
	decompile.h \
	libswf.h \
	output.h \
	read.h \
	swftophp.h 

listswf_SOURCES = listswf.c
listswf_LDADD = libutil.la
listswf_LDFALGS = $(ZLIB)



listswf_nd: blocktypes.o decompile.o read.o listswf.c
	${CC} -DNODECOMPILE listswf.c blocktypes.o decompile.o read.o -o listswf -lm -lz

swftophp_SOURCES = swftophp.c
swftophp_LDADD = libutil.la

makefdb_SOURCES = makefdb.c
makefdb_LDADD = libutil.la

gif2mask_SOURCES = gif2mask.c
gif2mask_LDFLAGS = $(GIFLIB) $(ZLIB)

png2dbl_SOURCES = png2dbl.c
png2dbl_LDFLAGS = $(PNGLIB)

gif2dbl_SOURCES = gif2dbl.c
gif2dbl_LDFLAGS = $(GIFLIB) $(ZLIB)

EXTRA_DIST = \
	README \
	bindump.c \
	hexdump.c \
	listfdb.c \
	listjpeg.c \
	listmp3.c 
