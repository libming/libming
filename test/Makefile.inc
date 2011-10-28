all: $(TESTS_PROGS)

if BUILD_PERL_EXTENSION 
PERLBINDINGS = perl
endif
if BUILD_PYTHON_EXTENSION 
PYTHONBINDINGS = python
endif
if BUILD_PHP_EXTENSION 
PHPBINDINGS = php
endif
if USE_CXX 
CXXBINDINGS = cxx
endif

BUILD_OPTIONS =

if USE_GIF
BUILD_OPTIONS += "GIF"
endif

if USE_PNG
BUILD_OPTIONS += "PNG"
endif

if USE_FREETYPE
BUILD_OPTIONS += "FREETYPE"
endif

BINDINGS = "c $(CXXBINDINGS) $(PERLBINDINGS) $(PYTHONBINDINGS) $(PHPBINDINGS)"

#
# Warning!!!!
# This is a cheat. Reference results should never be generated using the
# same code that it is testing. Due to the large number of tests involved,
# and the likelyhood of listswf output changing as things get fixed, we
# need a reasonable way to avoid having to hand edit a huge number of
# reference files. The compromise, is that all new reference files, and
# all changes to existing files must be carefully inspected by hand before
# committing them.
#
refs: $(C_PROGS)
	for t in $(C_PROGS); do ./$$t; $(top_builddir)/util/listswf $$t.swf > $(srcdir)/$$t.ref; done

gen-php: $(SWFS)
	for t in $(C_PROGS); do \
	$(top_builddir)/util/swftophp $$t.swf $$t.swf > $(srcdir)/$$t.php ;\
	done

gen-pl: $(SWFS) 
	for t in $(C_PROGS); do \
	$(top_builddir)/util/swftoperl $$t.swf $$t.swf > $(srcdir)/$$t.pl ;\
	done

gen-py: $(SWFS) 
	for t in $(C_PROGS); do \
	$(top_builddir)/util/swftopython $$t.swf $$t.swf > $(srcdir)/$$t.py ;\
	done

gen-cxx: $(SWFS) 
	for t in $(C_PROGS); do \
	$(top_builddir)/util/swftocxx $$t.swf $$t.swf > $(srcdir)/$$t-cxx.C ;\
	done

gen-tcl: $(SWFS) 
	for t in $(C_PROGS); do \
	$(top_builddir)/util/swftotcl $$t.swf $$t.swf > $(srcdir)/$$t.tcl ;\
	done

gen: refs $(SWFS) gen-php gen-pl gen-py gen-cxx gen-tcl

.PHONY: dotests 
dotests: $(top_srcdir)/test/dotests.pl
	echo "top_builddir=$(top_builddir) builddir=$(PWD) top_srcdir=$(top_srcdir) srcdir=$(srcdir) BINDINGS='$(BINDINGS)' BUILD_OPTIONS='$(BUILD_OPTIONS)' PERL='$(PERL)' PYTHON='$(PYTHON)' PHP='$(PHP)' $(PERL) -w $(top_srcdir)/test/dotests.pl" > $@
	chmod +x $@

.php.phpx:
	echo "#!/bin/sh" > $@
	echo "$(PHP) -n -d extension_dir=$(top_builddir)/php_ext/.libs -d extension=ming.so $< $(srcdir) && echo Done || echo Failed" >> $@
	chmod +x $@

CLEANFILES += *.phpx
