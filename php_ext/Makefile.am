EXTRA_DIST = config.m4  make_module.sh  ming.c  php_ming.h README.PHP4 \
	README.PHP5 ming.dsp

all: make_module.sh
	$(SHELL) $(srcdir)/make_module.sh $(srcdir)

clean-local:
	rm -rf tmp

install:
	$(INSTALL) -m 755 tmp/modules/ming.so `php-config --extension-dir`
