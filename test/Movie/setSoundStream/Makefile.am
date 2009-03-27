SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test \
	-DMEDIADIR='"$(srcdir)/../../Media/"'

LDADD = $(top_builddir)/src/libming.la

TESTS = dotests

C_PROGS =		\
	test01		\
	test02		\
	test03		\
	test04	

if USE_CXX
CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	\
	test03-cxx	\
	test04-cxx

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C
test03_cxx_SOURCES = test03-cxx.C
test04_cxx_SOURCES = test04-cxx.C
endif

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST =	\
	test01.ref \
	test01.pl \
	test01.php \
	test02.ref \
	test02.pl \
	test02.php \
	test03.ref \
	test03.pl \
	test03.php \
	test04.ref \
	test04.pl \
	test04.php \
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

CLEANFILES = dotests php.ini \
	test01.swf \
	test02.swf \
	test03.swf \
	test04.swf

include ../../Makefile.inc
