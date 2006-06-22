SUBDIRS = Movie

BINDINGS = c

if BUILD_PERL_EXTENSION
#BINDINGS += perl
endif

if BUILD_PYTHON_EXTENSION
#BINDINGS += python
endif

if BUILD_PHP_EXTENSION
BINDINGS += php
endif

# Currently, perl is required to run the ./dotest script
if HAVE_PERL

#TESTS_ENVIRONMENT = BINDINGS="$(BINDINGS)" $(PERL)
#TESTS = dotests

endif 

check_SCRIPTS = dotests

EXTRA_DIST = TestList 
