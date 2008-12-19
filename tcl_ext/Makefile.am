
#AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = ming_wrap.c

mingc_la_LTLIBRARIES = mingc.la
mingc_ladir  = $(libdir)/ming/tcl
nodist_mingc_la_SOURCES  = $(BUILT_SOURCES)
mingc_la_CPPFLAGS  = -I.. -I../src -I$(TCL_INC_DIR)
mingc_la_LDFLAGS  = -module -no-undefined -avoid-version
mingc_la_LIBADD  = ../src/libming.la

MAINTAINERCLEANFILES = ming_wrap.c

CLEANFILES = test.swf

EXTRA_DIST = test.tcl MANIFEST

ming_wrap.c: ../ming.i
	swig -I$(srcdir)/.. -tcl ming.i
