AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip 1.4d

if WITH_CXX
CXX_SUBDIR = c++_ext
else
CXX_SUBDIR = 
endif

if WITH_PYTHON
PYTHON_SUBDIR = py_ext
else
PYTHON_SUBDIR =
endif

if WITH_RUBY
RUBY_SUBDIR = rb_ext
else
RUBY_SUBDIR =
endif

if WITH_PERL
PERL_SUBDIR = perl_ext
else
PERL_SUBDIR =
endif

if WITH_PHP
PHP_SUBDIR = php_ext
else
PHP_SUBDIR =
endif

COMMON_SUBDIRS = src util

SUBDIRS = $(COMMON_SUBDIRS) $(CXX_SUBDIR) $(PYTHON_SUBDIR) $(RUBY_SUBDIR) $(PERL_SUBDIR) $(PHP_SUBDIR)

DIST_SUBDIRS = $(COMMON_SUBDIRS) py_ext

EXTRA_DIST = ming.i CREDITS CHANGES autogen.sh LICENSE

include_HEADERS = ming.h

dist-hook:
	cp -a perl_ext $(distdir)
	cp -a php_ext $(distdir)
	cp -a c++_ext $(distdir)
	cp -a rb_ext $(distdir)
	for i in `find $(distdir) -name "CVS" -type d`; do rm -Rf $$i; done

all-dist:
	$(MAKE) dist
	$(MAKE) dist-bzip2
	$(MAKE) dist-zip
