all: Makefile.perl
	$(MAKE) -f Makefile.perl

Makefile.perl: Makefile.PL
	$(PERL) $< MAKEFILE=Makefile.perl

clean-local:
	$(MAKE) -f Makefile.perl realclean
	rm -f Makefile.perl
