AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

EXTRA_DIST =  \
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	TestList

check_PROGRAMS = setRateTest

TESTS = setRateTest

setRateTest_SOURCES = setRateTest.c ../../run_test.c
setRateTest_LDADD = $(top_builddir)/src/libming.la
setRateTest_CFLAGS = -DTOP_BUILDDIR='"$(top_builddir)"' -DTOP_SOURCEDIR='"$(srcdir)"'
