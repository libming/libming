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
	test03		\
	test04		\
	test05		\
	test06

CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	\
	test03-cxx	\
	test04-cxx	\
	test05-cxx	\
	test06-cxx

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
	test06.py	\
	test06.php	\
	test06.pl	\
	test06.ref	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C
test03_cxx_SOURCES = test03-cxx.C
test04_cxx_SOURCES = test04-cxx.C
test05_cxx_SOURCES = test05-cxx.C
test06_cxx_SOURCES = test06-cxx.C

CLEANFILES = dotests test0*.swf php.ini

include ../../Makefile.inc
