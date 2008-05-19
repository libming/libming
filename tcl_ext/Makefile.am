
#AUTOMAKE_OPTIONS = foreign

mingc_la_LTLIBRARIES = mingc.la
mingc_ladir  = $(libdir)/ming/tcl
mingc_la_SOURCES  = ming_wrap.c
mingc_la_CPPFLAGS  = -I.. -I../src -I$(TCL_INC_DIR)
mingc_la_LDFLAGS  = -module -no-undefined -avoid-version
mingc_la_LIBADD  = ../src/libming.la

MAINTAINERCLEANFILES = ming_wrap.c

CLEANFILES = test.swf

EXTRA_DIST = test.tcl ming_wrap.c MANIFEST

ming_wrap.c: ../ming.i
	swig -I.. -tcl ming.i
