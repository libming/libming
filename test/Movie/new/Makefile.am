SUBDIRS = 

AM_CPPFLAGS = -I$(top_srcdir)/src

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
	test01 \
	test02 \
	test03 \
	test04 \
	test05

test01_SOURCES = test01.c
test01_LDADD = $(top_builddir)/src/libming.la

test02_SOURCES = test02.c
test02_LDADD = $(top_builddir)/src/libming.la

test03_SOURCES = test03.c
test03_LDADD = $(top_builddir)/src/libming.la

test04_SOURCES = test04.c
test04_LDADD = $(top_builddir)/src/libming.la

test05_SOURCES = test05.c
test05_LDADD = $(top_builddir)/src/libming.la
