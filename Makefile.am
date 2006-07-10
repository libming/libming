# NOTES on automake requirements:
#
# Testing and patching scripts as of Tue Jul 11 00:00:27 CEST 2006 we got:
#
#	automake 1.5 is known to work
#       automake 1.4-p6 is known *NOT* to work 
#
# Original automake contributor (David Schleef <ds@schleef.org>) set 
# requirement to 1.4d, but we didn't check it
#
AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip 1.4d

if BUILD_PERL_EXTENSION
PERLSUBDIR = perl_ext
endif

SUBDIRS =	\
        src	\
	$(PERLSUBDIR) \
	util	\
	docs	\
	macros	\
	test

include_HEADERS = mingpp.h

Changelog:
	cvs2cl --accum -I ChangeLog
