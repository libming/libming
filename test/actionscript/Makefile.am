SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

TESTS = ActionScriptTest

AS_TESTS = Function.as

EXTRA_DIST = 		\
	Function.ref	\
	$(AS_TESTS)

CLEANFILES = *.as.pp *.swf

check_PROGRAMS = \
	ActionScriptTest

ActionScriptTest_SOURCES = ActionScriptTest.c ../run_test.c ../../util/makeswf_utils.c
ActionScriptTest_LDADD = $(top_builddir)/src/libming.la
ActionScriptTest_CFLAGS = -DTOP_BUILDDIR='"$(top_builddir)"' -DTOP_SOURCEDIR='"$(srcdir)"' -I../../util/ -DAS_TESTS='"$(AS_TESTS)"'

