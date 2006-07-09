

INCLUDES = -I$(top_srcdir)/src

if USE_GIF
if USE_ZLIB
gifprograms = gif2mask gif2dbl
else
gifprograms =
endif
else
gifprograms =
endif

if USE_PNG
pngprograms = png2dbl png2swf dbl2png
else
pngprograms =
endif

if USE_FREETYPE
ft2programs = ttftofft
else
ft2programs =
endif

bin_PROGRAMS = \
	listswf \
	listswf_d \
	swftophp \
	swftoperl \
	swftopython \
	swftocxx \
	makeswf \
	listaction \
	listaction_d \
	listfdb \
	makefdb \
	listjpeg \
	raw2adpcm \
	listmp3 \
	listmp3 \
	$(ft2programs) \
	$(pngprograms) \
	$(gifprograms)

noinst_LTLIBRARIES = libutil.la

libutil_la_SOURCES =	\
	action.c	\
	blocktypes.c	\
	decompile.c	\
	main.c		\
	parser.c	\
	read.c 

libutil_la_LDFLAGS = $(MATHLIB) $(ZLIB)

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
	swftypes.h

listswf_SOURCES = outputtxt.c
listswf_LDADD = libutil.la $(top_builddir)/src/libming.la
listswf_CFLAGS = -DNODECOMPILE

listswf_d_SOURCES = outputtxt.c
listswf_d_LDADD = libutil.la $(top_builddir)/src/libming.la

swftophp_SOURCES = outputscript.c
swftophp_LDADD = libutil.la $(top_builddir)/src/libming.la
swftophp_CFLAGS = -DSWFPHP

swftoperl_SOURCES = outputscript.c
swftoperl_LDADD = libutil.la $(top_builddir)/src/libming.la
swftoperl_CFLAGS = -DSWFPERL

swftopython_SOURCES = outputscript.c
swftopython_LDADD = libutil.la $(top_builddir)/src/libming.la
swftopython_CFLAGS = -DSWFPYTHON

swftocxx_SOURCES = outputscript.c
swftocxx_LDADD = libutil.la $(top_builddir)/src/libming.la
swftocxx_CFLAGS = -DSWFPLUSPLUS

listaction_SOURCES = outputtxt.c
listaction_LDADD = libutil.la $(top_builddir)/src/libming.la
listaction_CFLAGS = -DACTIONONLY -DNODECOMPILE

listaction_d_SOURCES = outputtxt.c
listaction_d_LDADD = libutil.la $(top_builddir)/src/libming.la
listaction_d_CFLAGS = -DACTIONONLY 

ttftofft_SOURCES = ttftofft.c outputswf.c
ttftofft_LDADD = libutil.la $(top_builddir)/src/libming.la $(FREETYPE_LIBS)
ttftofft_CFLAGS = -DNODECOMPILE $(FREETYPE_CFLAGS)

listfdb_SOURCES = listfdb.c
listfdb_LDADD = $(top_builddir)/src/libming.la

raw2adpcm_SOURCES = raw2adpcm.c
raw2adpcm_LDADD = $(top_builddir)/src/libming.la

listmp3_SOURCES = listmp3.c
listmp3_LDADD = $(top_builddir)/src/libming.la

listjpeg_SOURCES = listjpeg.c
listjpeg_LDADD = $(top_builddir)/src/libming.la

makefdb_SOURCES = outputfdb.c
makefdb_LDADD = libutil.la $(top_builddir)/src/libming.la

gif2mask_SOURCES = gif2mask.c
gif2mask_LDADD = $(top_builddir)/src/libming.la

png2dbl_SOURCES = png2dbl.c
png2dbl_LDADD = $(top_builddir)/src/libming.la

png2swf_SOURCES = png2swf.c
png2swf_LDADD = $(top_builddir)/src/libming.la

dbl2png_SOURCES = dbl2png.c
dbl2png_LDADD = $(top_builddir)/src/libming.la

gif2dbl_SOURCES = gif2dbl.c
gif2dbl_LDADD = $(top_builddir)/src/libming.la

makeswf_SOURCES = makeswf.c makeswf_utils.c
makeswf_LDADD = $(top_builddir)/src/libming.la

bin_SCRIPTS = ming-config

EXTRA_DIST = \
	README \
	bindump.c \
	hexdump.c 
