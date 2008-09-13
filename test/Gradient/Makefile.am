SUBDIRS = 

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test

LDADD = $(top_builddir)/src/libming.la

TESTS = dotests

C_PROGS =		\
	test01		\
	test02		\
	test03		\
	test04		\
	test05		\
	test06		\
	test07		\
	test08		\
	test09	

if USE_CXX
CXX_PROGS =		\
	test01-cxx	\
	test02-cxx	\
	test03-cxx	\
	test04-cxx	\
	test05-cxx	\
	test06-cxx	\
	test07-cxx	\
	test08-cxx	\
	test09-cxx

test01_cxx_SOURCES = test01-cxx.C
test02_cxx_SOURCES = test02-cxx.C
test03_cxx_SOURCES = test03-cxx.C
test04_cxx_SOURCES = test04-cxx.C
test05_cxx_SOURCES = test05-cxx.C
test06_cxx_SOURCES = test06-cxx.C
test07_cxx_SOURCES = test07-cxx.C
test08_cxx_SOURCES = test08-cxx.C
test09_cxx_SOURCES = test09-cxx.C
endif

TEST_PROGS = $(C_PROGS) $(CXX_PROGS)

EXTRA_DIST = 		\
	test01.ref	\
	test02.ref	\
	test03.ref	\
	test04.ref	\
	test05.ref	\
	test06.ref	\
	test07.ref	\
	test08.ref	\
	test09.ref	\
	TestList

check_PROGRAMS = \
	$(TEST_PROGS)

CLEANFILES = dotests php.ini \
	test01.swf \
	test02.swf \
	test03.swf \
	test04.swf \
	test05.swf \
	test06.swf \
	test07.swf \
	test08.swf \
	test09.swf

include ../Makefile.inc
