SUBDIRS = 

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/test

EXTRA_DIST = \
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	TestList

check_PROGRAMS = setDimensionTest

TESTS = setDimensionTest

setDimensionTest_SOURCES = setDimensionTest.c ../../run_test.c
setDimensionTest_LDADD = $(top_builddir)/src/libming.la
setDimensionTest_CFLAGS = -DTOP_BUILDDIR='"$(top_builddir)"' -DTOP_SOURCEDIR='"$(srcdir)"'
