
all install dynamic static clean distclean: config.log
	$(MAKE) -f Makefile-real $@

config.log:
	./configure

