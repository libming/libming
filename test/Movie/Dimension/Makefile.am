SUBDIRS = 

AM_CPPFLAGS = -I$(top_srcdir)/src

EXTRA_DIST = \
	test01.php	\
	test01.pl	\
	test01.py	\
	test01.ref	\
	TestList

check_PROGRAMS = test01

test01_SOURCES = test01.c
test01_LDADD = $(top_builddir)/src/libming.la
