
RULES = all install install-static install-dynamic \
	dynamic static clean distclean Changelog \
	release maintainer-clean check

$(RULES): Makefile.config
	$(MAKE) -f Makefile-real $@

config.log Makefile.config: config.status Makefile.config.in 
	./config.status

config.status: 
	./configure

