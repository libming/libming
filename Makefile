
all install dynamic static clean distclean: Rules.make config.make
	$(MAKE) -f Makefile-real $@

config.log Rules.make config.make: config.status Rules.make.in config.make.in
	./config.status

config.status: 
	./configure

