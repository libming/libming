

INCLUDES = -I$(top_srcdir)/src

if USE_GIF
if USE_ZLIB
gifprograms = gif2mask gif2dbl
endif
else
gifprograms =
endif

if USE_PNG
if USE_ZLIB
pngprograms = png2dbl dbl2png
endif
else
pngprograms =
endif

bin_PROGRAMS = \
	listswf \
	listswf_d \
	swftophp \
	swftoperl \
	swftopython \
	swftocxx \
	swftotcl \
	makeswf \
	listaction \
	listaction_d \
	listfdb \
	makefdb \
	listjpeg \
	raw2adpcm \
	listmp3 \
	listmp3 \
	$(pngprograms) \
	$(gifprograms)

noinst_LTLIBRARIES = libutil.la

libutil_la_SOURCES =	\
	action.c	\
	blocktypes.c	\
	decompile.c	\
	parser.c	\
	read.c 		\
	vasprintf.c

libutil_la_LIBADD = $(MATHLIB) $(ZLIB)

noinst_HEADERS = \
	action.h	\
	decompile.h	\
	libswf.h	\
	makeswf.h	\
	outputdecl.h	\
	output.h	\
	parserdecl.h	\
	parser.h	\
	read.h		\
	swfoutput.h	\
	swftypes.h	\
	vasprintf.h

listswf_SOURCES = outputtxt.c main.c	
listswf_LDADD = libutil.la $(top_builddir)/src/libming.la
listswf_CFLAGS = -DNODECOMPILE

listswf_d_SOURCES = outputtxt.c main.c
listswf_d_LDADD = libutil.la $(top_builddir)/src/libming.la

swftophp_SOURCES = outputscript.c main.c
swftophp_LDADD = libutil.la $(top_builddir)/src/libming.la
swftophp_CFLAGS = -DSWFPHP

swftoperl_SOURCES = outputscript.c main.c
swftoperl_LDADD = libutil.la $(top_builddir)/src/libming.la
swftoperl_CFLAGS = -DSWFPERL

swftopython_SOURCES = outputscript.c main.c
swftopython_LDADD = libutil.la $(top_builddir)/src/libming.la
swftopython_CFLAGS = -DSWFPYTHON

swftocxx_SOURCES = outputscript.c main.c
swftocxx_LDADD = libutil.la $(top_builddir)/src/libming.la
swftocxx_CFLAGS = -DSWFPLUSPLUS

swftotcl_SOURCES = outputscript.c main.c
swftotcl_LDADD = libutil.la $(top_builddir)/src/libming.la
swftotcl_CFLAGS = -DSWFTCL

listaction_SOURCES = outputtxt.c main.c
listaction_LDADD = libutil.la $(top_builddir)/src/libming.la
listaction_CFLAGS = -DACTIONONLY -DNODECOMPILE

listaction_d_SOURCES = outputtxt.c main.c
listaction_d_LDADD = libutil.la $(top_builddir)/src/libming.la
listaction_d_CFLAGS = -DACTIONONLY 

listfdb_SOURCES = listfdb.c
listfdb_LDADD = $(top_builddir)/src/libming.la

raw2adpcm_SOURCES = raw2adpcm.c
raw2adpcm_LDADD = $(top_builddir)/src/libming.la

listmp3_SOURCES = listmp3.c
listmp3_LDADD = $(top_builddir)/src/libming.la

listjpeg_SOURCES = listjpeg.c
listjpeg_LDADD = $(top_builddir)/src/libming.la

makefdb_SOURCES = outputfdb.c main.c
makefdb_LDADD = libutil.la $(top_builddir)/src/libming.la
makefdb_CFLAGS = -DMAKE_FDB

gif2mask_SOURCES = gif2mask.c
gif2mask_LDADD = $(top_builddir)/src/libming.la $(MATHLIB) $(ZLIB) $(GIFLIB)

png2dbl_SOURCES = png2dbl.c
png2dbl_LDADD = $(top_builddir)/src/libming.la $(MATHLIB) $(ZLIB) $(PNGLIB)

dbl2png_SOURCES = dbl2png.c
dbl2png_LDADD = $(top_builddir)/src/libming.la $(ZLIB) $(PNGLIB)

gif2dbl_SOURCES = gif2dbl.c
gif2dbl_LDADD = $(top_builddir)/src/libming.la $(MATHLIB) $(ZLIB) $(GIFLIB)

makeswf_SOURCES = makeswf.c makeswf_utils.c
makeswf_LDADD = libutil.la $(top_builddir)/src/libming.la

bin_SCRIPTS = ming-config

EXTRA_DIST = \
	README \
	TIPS \
	TODO \
	bindump.c \
	hexdump.c \
	cws2fws \
	swftoperl.html \
	ming.css

DISTCLEANFILES = ming-config
