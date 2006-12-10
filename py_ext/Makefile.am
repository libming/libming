EXTRA_DIST = Zwiff/README.txt Zwiff/__init__.py Zwiff/setFlags.py \
		INSTALL README TODO ming.py ming_wrap.c \
		mingc.def mingc.py setup.py shape.py MANIFEST

all: build
	@echo "This Makefile is not yet edited"

clean:
	@rm -Rf test.swf build


build:
	$(PYTHON) setup.py build


install:
	$(PYTHON) setup.py install $(PREFIX)

test.swf:
	$(PYTHON) shape.py
