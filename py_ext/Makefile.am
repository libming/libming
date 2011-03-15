EXTRA_DIST = Zwiff/README.txt Zwiff/__init__.py Zwiff/setFlags.py \
		INSTALL README TODO ming.py ming_wrap.c \
		mingc.def mingc.py setup.py shape.py MANIFEST

BUILT_SOURCES = ming_wrap.c

SWIG_GENERATED = ming_wrap.c mingc.py

MAINTAINER_CLEAN = $(SWIG_GENERATED)

all-local:
	CFLAGS="$(CFLAGS) -fno-strict-aliasing" $(PYTHON) setup.py build

ming_wrap.c: ../ming.i
	swig -I$(srcdir)/.. -python ming.i

clean-local:
	rm -Rf test.swf build

install:
	$(PYTHON) setup.py install --prefix $(prefix) --root "/$(DESTDIR)"

test.swf:
	$(PYTHON) shape.py
