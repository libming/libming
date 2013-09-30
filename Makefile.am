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
ACLOCAL_AMFLAGS = -I macros

additionnal_dist_subdirs = 

if BUILD_PERL_EXTENSION
PERLSUBDIR = perl_ext
else
additionnal_dist_subdirs += perl_ext
endif

if BUILD_PHP_EXTENSION
PHPSUBDIR = php_ext
else
additionnal_dist_subdirs += php_ext
endif

if BUILD_PYTHON_EXTENSION
PYSUBDIR = py_ext
else
additionnal_dist_subdirs += py_ext
endif

if BUILD_TCL_EXTENSION
TCLSUBDIR = tcl_ext
else
additionnal_dist_subdirs += tcl_ext
endif

SUBDIRS =	\
        src	\
	$(PERLSUBDIR) \
	$(PHPSUBDIR) \
	$(PYSUBDIR) \
	$(TCLSUBDIR) \
	util	\
	docs	\
	macros	\
	test

DIST_SUBDIRS = $(SUBDIRS) $(additionnal_dist_subdirs)

if USE_CXX
include_HEADERS = mingpp.h
endif

COPYING: LICENSE
	ln -fs $(srcdir)/LICENSE COPYING

.PHONY: rpm ChangeLog
rpm: dist
	rpmbuild -ta ming-@VERSION@.tar.gz

EXTRA_DIST = ming.spec ming.spec.in ming.i COPYING LICENSE LICENSE_GPL2 \
	autogen.sh HISTORY
DISTCLEANFILES = ming-@VERSION@* ming.spec COPYING

ChangeLog Changelog:
	cd $(srcdir) && git2cl > ChangeLog 

# alias to make travis config easier
test: check
