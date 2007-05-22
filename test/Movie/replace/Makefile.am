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
	test02		

CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.ref	\
	test02.ref	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C

CLEANFILES = dotests test0*.swf php.ini

include ../../Makefile.inc
