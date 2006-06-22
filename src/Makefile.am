SUBDIRS = \
	actioncompiler \
	blocks

noinst_HEADERS =	\
	actiontypes.h	\
	blocklist.h	\
	displaylist.h	\
	fill.h		\
	font_util.h	\
	gc.h		\
	libming.h	\
	ming_config.h	\
	movieclip.h	\
	movie.h		\
	position.h	\
	shape_cubic.h	\
	shape_util.h	\
	text_util.h

lib_LTLIBRARIES = libming.la

include_HEADERS = ming.h ming_config.h

libming_la_SOURCES = 	\
	blocklist.c	\
	displaylist.c	\
	fill.c		\
	font_util.c	\
	gc.c		\
	ming.c		\
	movie.c		\
	movieclip.c	\
	position.c	\
	shape_cubic.c	\
	shape_util.c	\
	test.c		\
	text_util.c

libming_la_LDFLAGS = \
	-version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE) \
        -no-undefined \
	$(PNGLIB) \
	$(GIFLIB) \
	$(ZLIB)

libming_la_LIBADD = \
	actioncompiler/libactioncompiler.la \
	blocks/libblock.la

