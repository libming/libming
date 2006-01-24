
RELEASE=rel-not-set

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

Changelog::
	cvs2cl --accum
#
# Make release tarballs. Note, the license on the php & java extension
# doesn't sit well with some distros (not DFSG compatible), so we'll
# just packages them seperately to make things convenient.
#
release:
	cvs export -d ming-$(RELEASE) -D now ming
	tar czvf ming-ch-$(RELEASE).tar.gz ming-$(RELEASE)/ch
	rm -rf ming-$(RELEASE)/ch
	tar czvf ming-rb-$(RELEASE).tar.gz ming-$(RELEASE)/rb_ext
	rm -rf ming-$(RELEASE)/rb_ext
	tar czvf ming-py-$(RELEASE).tar.gz ming-$(RELEASE)/py_ext
	rm -rf ming-$(RELEASE)/py_ext
	tar czvf ming-tcl-$(RELEASE).tar.gz ming-$(RELEASE)/tcl_ext
	rm -rf ming-$(RELEASE)/tcl_ext
	tar czvf ming-perl-$(RELEASE).tar.gz ming-$(RELEASE)/perl_ext
	rm -rf ming-$(RELEASE)/perl_ext
	tar czvf ming-java-$(RELEASE).tar.gz ming-$(RELEASE)/java_ext
	rm -rf ming-$(RELEASE)/java_ext
	tar czvf ming-php-$(RELEASE).tar.gz ming-$(RELEASE)/php_ext
	rm -rf ming-$(RELEASE)/php_ext
	tar czvf ming-$(RELEASE).tar.gz ming-$(RELEASE)
	rm -rf ming-$(RELEASE)
