include ../Makefile.config

OFILES = \
	blocklist.o \
	displaylist.o \
	fill.o \
	font_util.o \
	ming.o \
	movie.o \
	movieclip.o \
	position.o \
	shape_cubic.o \
	shape_util.o \
	text_util.o \
	gc.o

COMPILEROFILES = actioncompiler/compile.o \
		 actioncompiler/swf4compiler.tab.o \
		 actioncompiler/lex.swf4.o \
		 actioncompiler/swf5compiler.tab.o \
		 actioncompiler/lex.swf5.o \
		 actioncompiler/compileaction.o \
		 actioncompiler/assembler.o

all: dynamic

# saveFP and restFP on OS X can be retrieved as such:
# ar -x /usr/lib/libcc_dynamic.a lib/macosx-fpsave.o

dynamic: $(SHAREDLIB)

$(SHAREDLIB): $(OFILES)
	cd blocks && $(MAKE)
	cd actioncompiler && $(MAKE)
	$(CC) $(OFILES) $(COMPILEROFILES) $(LDFLAGS) $(EXTRA_LIBS) blocks/*.o $(SHLIBLDFLAGS) -o $(SHAREDLIB)
	cp $(SHAREDLIB) ..
	ln -fs $(SHAREDLIB) ../libming.so
	ln -fs $(SHAREDLIB) ../libming.so.$(MAJOR_VERSION)

static: libming.a

libming.a: $(OFILES)
	cd blocks && $(MAKE)
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o ${OFILES} ${COMPILEROFILES}
	ranlib libming.a  # bsd requires this?
	cp libming.a ..

clean:
	cd blocks && $(MAKE) clean
	cd actioncompiler && $(MAKE) clean
	rm -f *.o libming.a $(SHAREDLIB) libming.so test test.exe core *.core

# breaks on bsd:
#.c.o: .c .h
#	${CC} ${FLAGS} -c $<

test: static test.o
	${CC} -o test test.o libming.a  ${EXTRA_LIBS} ${LDFLAGS}
