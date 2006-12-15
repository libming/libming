SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

LDADD = $(top_builddir)/src/libming.la
LIBS=-lstdc++

TESTS = dotests

C_PROGS =		\
	test01		\
	test02		\
	$(NULL)

CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	\
	$(NULL)

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	test02.php	\
	test02.pl	\
	test02.py	\
	test02.ref	\
	TestList	\
	$(NULL)

check_PROGRAMS = \
	$(TEST_PROGS)

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C

CLEANFILES = dotests test0*.swf php.ini

include ../../Makefile.inc
