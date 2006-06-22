SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

EXTRA_DIST = \
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	TestList

check_PROGRAMS = \
	setNumberOfFramesTest

TESTS_ENVIRONMENT = top_builddir=$(top_builddir)
TESTS = setNumberOfFramesTest

setNumberOfFramesTest_SOURCES = setNumberOfFramesTest.c ../../run_test.c
setNumberOfFramesTest_LDADD = $(top_builddir)/src/libming.la
