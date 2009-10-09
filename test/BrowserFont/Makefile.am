SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

LDADD = $(top_builddir)/src/libming.la

TESTS = dotests

C_PROGS =		\
	test01		

if USE_CXX
CXX_PROGS =		\
	test01-cxx	

test01_cxx_SOURCES = test01-cxx.C
endif

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

CLEANFILES = dotests php.ini \
	test01.swf 

include ../Makefile.inc
