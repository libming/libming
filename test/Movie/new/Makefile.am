SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

TESTS_ENVIRONMENT = top_builddir=$(top_builddir)
TESTS = newSWFMovieTest

EXTRA_DIST = 		\
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	test02.php	\
	test02.pl	\
	test02.py	\
	test02.ref	\
	test03.php	\
	test03.pl	\
	test03.py	\
	test03.ref	\
	test04.php	\
	test04.pl	\
	test04.py	\
	test04.ref	\
	test05.php	\
	test05.pl	\
	test05.py	\
	test05.ref	\
	TestList

check_PROGRAMS = \
	newSWFMovieTest

newSWFMovieTest_SOURCES = newSWFMovieTest.c ../../run_test.c
newSWFMovieTest_LDADD = $(top_builddir)/src/libming.la

