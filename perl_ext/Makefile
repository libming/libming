USRLIBPERL=/usr/lib/perl5/5.00503
SITEPERL=/usr/lib/perl5/site_perl/5.005/i386-linux

all:
	swig -I.. -perl5 ming.i
	gcc -fpic -Dbool=char -I.. -I${USRLIBPERL}/i386-linux/CORE \
	  -c ming_wrap.c
	gcc -L.. -shared ming_wrap.o -lming -o mingc.so

clean:
	rm -f mingc.so mingc.pm core *~ *.o ming_wrap.o mingcmodule.so

install:
	echo WARNING: make install is untested.
	mkdir -p ${SITEPERL}/auto/mingc
	cp mingc.so ${SITEPERL}/auto/mingc
	cp mingc.pm ${SITEPERL}
	chmod 755 ${SITEPERL}/auto/mingc/mingc.so
	chmod 755 ${SITEPERL}/mingc.pm
