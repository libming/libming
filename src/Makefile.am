AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip 1.4d

SUBDIRS = actioncompiler blocks

lib_LTLIBRARIES = libming.la
EXTRA_PROGRAMS = test

libming_la_SOURCES = \
	movie.c \
	blocklist.c \
	displaylist.c \
	fill.c \
	font_util.c \
	gc.c \
	ming.c \
	movie.c \
	movieclip.c \
	position.c \
	shape_cubic.c \
	shape_util.c \
	test.c \
	text_util.c

noinst_HEADERS = \
	blocklist.h \
	blocktypes.h \
	displaylist.h \
	fill.h \
	font_util.h \
	gc.h \
	libming.h \
	ming.h \
	movie.h \
	movieclip.h \
	position.h \
	shape_util.h \
	shape_cubic.h \
	text_util.h

libming_la_LIBADD = actioncompiler/libactioncompiler.la blocks/libswf.la
libming_la_LDFLAGS = -version-info 1:0:0 $(MATHLIB)
libming_la_CFLAGS = -I$(srcdir)


test_SOURCES = test.c
test_LDADD = libming.la
