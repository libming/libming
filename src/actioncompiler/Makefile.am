AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip 1.4d

LEXLIB = @LEXLIB@

AM_YFLAGS = -d

noinst_LTLIBRARIES = libactioncompiler.la
libactioncompiler_la_SOURCES = \
	assembler.c \
	compile.c \
	listaction.c \
	compileaction.c
nodist_libactioncompiler_la_SOURCES = \
	swf4compiler.y \
	swf5compiler.y \
	lex.swf4.c \
	lex.swf5.c

noinst_HEADERS = \
	action.h \
	assembler.h \
	compile.h

libactioncompiler_la_LIBADD = $(LEXLIB)

AM_CFLAGS = -I$(srcdir)/..

#noinst_PROGRAMS = test
#test_SOURCES = main.c 
#test_LDADD = libactioncompiler.la

EXTRA_DIST = swf4compiler.flex \
	swf4compiler.y \
	swf5compiler.flex \
	swf5compiler.y

CLEANFILES = swf4compiler.output \
	swf4compiler.c \
	swf4compiler.h \
	swf5compiler.c \
	swf5compiler.output \
	swf5compiler.h \
	lex.swf4.c \
	lex.swf5.c

lex.swf4.c: $(srcdir)/swf4compiler.flex swf4compiler.h
	$(LEX) -i -Pswf4 $(srcdir)/swf4compiler.flex

lex.swf5.c: $(srcdir)/swf5compiler.flex swf5compiler.h
	$(LEX) -i -Pswf5 $(srcdir)/swf5compiler.flex

swf4compiler.c swf4compiler.h: $(srcdir)/swf4compiler.y
	bison -d -v -pswf4 -o swf4compiler.c $(srcdir)/swf4compiler.y

swf5compiler.c swf5compiler.h: $(srcdir)/swf5compiler.y
	bison -d -v -pswf5 -o swf5compiler.c $(srcdir)/swf5compiler.y
