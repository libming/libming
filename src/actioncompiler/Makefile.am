
LEXLIB = @LEXLIB@

DEBUG = --debug

AM_YFLAGS = -d

# seems that the default is to add BUILT_SOURCES in MAINTAINERCLEANFILES,
# which is what we want
BUILT_SOURCES = \
	swf4compiler.tab.c \
	swf4compiler.tab.h \
	swf5compiler.tab.c \
	swf5compiler.tab.h \
	lex.swf4.c \
	lex.swf5.c

noinst_LTLIBRARIES = libactioncompiler.la

libactioncompiler_la_SOURCES = \
	assembler.c \
	compile.c \
	listaction.c \
	compileaction.c \
	$(BUILT_SOURCES)

noinst_HEADERS = \
	assembler.h \
	compile.h

libactioncompiler_la_LIBADD = # $(LEXLIB)

AM_CPPFLAGS = -I$(srcdir)/..

#noinst_PROGRAMS = test
#test_SOURCES = main.c 
#test_LDADD = libactioncompiler.la

EXTRA_DIST = \
	swf4compiler.flex \
	swf4compiler.y \
	swf5compiler.flex \
	swf5compiler.y

CLEANFILES = \
	swf4compiler.output \
	$(BUILT_SOURCES)


lex.swf4.c: $(srcdir)/swf4compiler.flex swf4compiler.tab.h
	$(LEX) -Pswf4 $(srcdir)/swf4compiler.flex

lex.swf5.c: $(srcdir)/swf5compiler.flex swf5compiler.tab.h
	$(LEX) -Pswf5 $(srcdir)/swf5compiler.flex

swf4compiler.tab.c: $(srcdir)/swf4compiler.y
	$(YACC) -p swf4 -b swf4compiler $(srcdir)/swf4compiler.y

swf4compiler.tab.h: $(srcdir)/swf4compiler.y
	$(YACC) --defines $(DEBUG) -p swf4 -b swf4compiler $(srcdir)/swf4compiler.y

swf5compiler.tab.c: $(srcdir)/swf5compiler.y
	$(YACC) -p swf5 -b swf5compiler $(srcdir)/swf5compiler.y

swf5compiler.tab.h: $(srcdir)/swf5compiler.y
	$(YACC) --defines $(DEBUG) -p swf5 -b swf5compiler $(srcdir)/swf5compiler.y
