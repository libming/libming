
RULES = all install install-static install-dynamic \
	dynamic static clean distclean

$(RULES): Makefile.config
	$(MAKE) -f Makefile-real $@

config.log Makefile.config: config.status Makefile.config.in 
	./config.status

config.status: 
	./configure

maintainer-clean: distclean
	rm -f configure
