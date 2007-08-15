EXTRA_DIST = Zwiff/README.txt Zwiff/__init__.py Zwiff/setFlags.py \
		INSTALL README TODO ming.py ming_wrap.c \
		mingc.def mingc.py setup.py shape.py MANIFEST

all:
	CFLAGS="$(CFLAGS) -fno-strict-aliasing" $(PYTHON) setup.py build

clean-local:
	rm -Rf test.swf build

install:
	$(PYTHON) setup.py install --prefix $(prefix) --root "$(DESTDIR)"

test.swf:
	$(PYTHON) shape.py
