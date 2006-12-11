
#AUTOMAKE_OPTIONS = foreign

mingc_la_LTLIBRARIES = mingc.la
mingc_ladir  = $(libdir)
mingc_la_SOURCES  = ming_wrap.c
mingc_la_CPPFLAGS  = -I.. -I../src -I@TCL_INC_DIR@
mingc_la_LDFLAGS  = -module -no-undefined
mingc_la_LIBADD  = -lming

MAINTAINERCLEANFILES = ming_wrap.c

EXTRA_DIST = test.tcl ming_wrap.c

ming_wrap.c: ../ming.i
	swig -I.. -tcl ming.i
