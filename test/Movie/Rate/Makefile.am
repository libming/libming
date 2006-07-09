SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

LDADD = $(top_builddir)/src/libming.la
LIBS=-lstdc++

if USE_PERL
PERLBINDINGS = perl
endif
if USE_PYTHON
PYTHONBINDINGS = python
endif
if USE_PHP
PHPBINDINGS = php
endif
BINDINGS = "c cxx $(PERLBINDINGS) $(PYTHONBINDINGS) $(PHPBINDINGS)"

TESTS_ENVIRONMENT = top_builddir=$(top_builddir) BINDINGS=$(BINDINGS)

TESTS = ../../dotests

C_PROGS =		\
	test01

CXX_PROGS =		\
	test01-cxx

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

test01_cxx_SOURCES = test01-cxx.C

CLEANFILES = test01.swf

include ../../Makefile.inc
