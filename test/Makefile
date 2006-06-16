include ../Makefile.config

check:
	@if test x"$(PERL)" = x; then \
		echo "ERROR: perl is needed ro run unit tests, but configure couldn't find it." >&2; \
		exit 1; \
	else \
		$(PERL) ./dotests c; \
	fi 
