AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip 1.4d

LEXLIB = @LEXLIB@

AM_YFLAGS = -d

noinst_LTLIBRARIES = libactioncompiler.la
libactioncompiler_la_SOURCES = \
	compile.c \
	listaction.c \
	swf4compiler.y \
	swf5compiler.y \
	lex.swf4.c \
	lex.swf5.c \
	compileaction.c

noinst_HEADERS = \
	action.h \
	compile.h

libactioncompiler_la_LIBADD = $(LEXLIB)

EXTRA_PROGRAMS = test
test_SOURCES = main.c 
test_LDADD = libactioncompiler.la

EXTRA_DIST = swf4compiler.lex \
	swf5compiler.lex

lex.swf4.c: swf4compiler.lex
	$(LEX) -i -Pswf4 swf4compiler.lex

lex.swf5.c: swf5compiler.lex
	$(LEX) -i -Pswf5 swf5compiler.lex
