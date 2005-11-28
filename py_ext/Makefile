include ../Makefile.config

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
