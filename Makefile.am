AUTOMAKE_OPTIONS = foreign

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

COMMON_SUBDIRS = src util c++_ext

noinst_HEADERS = mingpp.h

SUBDIRS = $(COMMON_SUBDIRS) $(PYTHON_SUBDIR) $(RUBY_SUBDIR) $(PERL_SUBDIR) $(PHP_SUBDIR)

DIST_SUBDIRS = $(COMMON_SUBDIRS)

java_files = $(shell sed s,^,java_ext/, $(top_srcdir)/java_ext/MANIFEST)
perl_files = $(shell sed s,^,perl_ext/, $(top_srcdir)/perl_ext/MANIFEST)
php_files = $(shell sed s,^,php_ext/, $(top_srcdir)/php_ext/MANIFEST)
py_files = $(shell sed s,^,py_ext/, $(top_srcdir)/py_ext/MANIFEST)
rb_files = $(shell sed s,^,rb_ext/, $(top_srcdir)/rb_ext/MANIFEST)
tcl_files = $(shell sed s,^,tcl_ext/, $(top_srcdir)/tcl_ext/MANIFEST)

EXTRA_DIST = ming.i CREDITS autogen.sh LICENSE \
	$(java_files) $(perl_files) $(py_files) \
	$(php_files) $(rb_files) $(tcl_files)

