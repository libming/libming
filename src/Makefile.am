AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip 1.4d

SUBDIRS = actioncompiler blocks

lib_LTLIBRARIES = libming.la
EXTRA_PROGRAMS = test

libming_la_SOURCES = \
	movie.c \
	displaylist.c \
	blocklist.c \
	position.c \
	movieclip.c \
	shape_util.c \
	text_util.c \
	ming.c 

noinst_HEADERS = \
	blocklist.h \
	blocktypes.h \
	displaylist.h \
	fill.h \
	libming.h \
	ming.h \
	movie.h \
	movieclip.h \
	position.h \
	shape_util.h \
	text_util.h 

libming_la_LIBADD = actioncompiler/libactioncompiler.la blocks/libswf.la
libming_la_LDFLAGS = -version-info 1:0:0 $(MATHLIB)


test_SOURCES = test.c
test_LDADD = libming.la
