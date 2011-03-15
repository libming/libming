SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test \
	-DMEDIADIR='"$(srcdir)/../Media/"'

LDADD = $(top_builddir)/src/libming.la

TESTS = dotests

C_PROGS =		\
	test01		\
	test02	

if USE_CXX
CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C
endif

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.ref	\
	test01.pl	\
	test01.php	\
	test01.py	\
	test02.ref	\
	test02.pl	\
	test02.php	\
	test02.py	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

CLEANFILES = dotests php.ini \
	test01.swf	\
	test02.swf 

include ../Makefile.inc
