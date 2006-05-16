CFLAGS=-I ../../../src
LDFLAGS=-L../../.. -lming

TESTS = test01
SWFS = test01.swf

all: $(TESTS)

refs:
	for t in $(TESTS); do listswf $$t.swf >$$t.ref; done

clean:
	$(RM) $(SWFS) $(TESTS)
