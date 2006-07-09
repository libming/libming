SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

LDADD = $(top_builddir)/src/libming.la
LIBS=-lstdc++

@HAVE_PERL_TRUE@PERLBINDINGS = perl
@HAVE_PYTHON_TRUE@PYTHONBINDINGS = python
@HAVE_PHP_TRUE@PHPBINDINGS = php
BINDINGS = "c cxx $(PERLBINDINGS) $(PYTHONBINDINGS) $(PHPBINDINGS)"

TESTS_ENVIRONMENT = top_builddir=$(top_builddir) BINDINGS=$(BINDINGS)

TESTS = ../../dotests

C_PROGS =		\
	test01		\
	test02		\
	test03		\
	test04		\
	test05

CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	\
	test03-cxx	\
	test04-cxx	\
	test05-cxx

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
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C
test03_cxx_SOURCES = test03-cxx.C
test04_cxx_SOURCES = test04-cxx.C
test05_cxx_SOURCES = test05-cxx.C

CLEANFILES = test0*.swf

include ../../Makefile.inc
