all: Makefile.perl
	$(MAKE) -f Makefile.perl

Makefile.perl: Makefile.PL
	srcdir=$(srcdir) $(PERL) $< MAKEFILE=Makefile.perl

clean-local:
	$(MAKE) -f Makefile.perl realclean
	rm -f Makefile.perl
