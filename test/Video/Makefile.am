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
	test02		\
	test03		\
	test04		\
	test05		\
	test06	

if USE_CXX
CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	\
	test03-cxx	\
	test04-cxx	\
	test05-cxx	\
	test06-cxx	

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C
test03_cxx_SOURCES = test03-cxx.C
test04_cxx_SOURCES = test04-cxx.C
test05_cxx_SOURCES = test05-cxx.C
test06_cxx_SOURCES = test06-cxx.C
endif

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.php	\
	test02.php	\
	test03.php	\
	test04.php	\
	test05.php	\
	test06.php	\
	test01.pl	\
	test02.pl	\
	test03.pl	\
	test04.pl	\
	test05.pl	\
	test06.pl	\
	test01.py	\
	test02.py	\
	test03.py	\
	test04.py	\
	test05.py	\
	test06.py	\
	test01.ref	\
	test02.ref	\
	test03.ref	\
	test04.ref	\
	test05.ref	\
	test06.ref	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

CLEANFILES = dotests php.ini \
	test01.swf	\
	test02.swf	\
	test03.swf	\
	test04.swf 	\
	test05.swf	\
	test06.swf

include ../Makefile.inc
