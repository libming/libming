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

DOCSRC = \
	blocks/action.c \
	blocks/bitmap.c \
	blocks/block.c \
	blocks/browserfont.c \
	blocks/button.c \
	blocks/character.c \
	blocks/cxform.c \
	blocks/dbl.c \
	blocks/error.c \
	blocks/fillstyle.c \
	blocks/font.c \
	blocks/fontinfo.c \
	blocks/fromswf.c \
	blocks/gifdbl.c \
	blocks/gradient.c \
	blocks/imports.c \
	blocks/input.c \
	blocks/jpeg.c \
	blocks/linestyle.c \
	blocks/matrix.c \
	blocks/method.c \
	blocks/morph.c \
	blocks/mp3.c \
	blocks/outputblock.c \
	blocks/output.c \
	blocks/placeobject.c \
	blocks/pngdbl.c \
	blocks/read.c \
	blocks/rect.c \
	blocks/shape.c \
	blocks/sound.c \
	blocks/soundinstance.c \
	blocks/soundstream.c \
	blocks/sprite.c \
	blocks/text.c \
	blocks/textfield.c \
	blocks/ttffont.c \
	blocks/utf8.c \
	blocks/videostream.c \
	blocklist.c \
	displaylist.c \
	fill.c \
	font_util.c \
	gc.c \
	ming.c \
	movie.c \
	movieclip.c \
	position.c \
	shape_cubic.c \
	shape_util.c \
	text_util.c

all: dynamic

# saveFP and restFP on OS X can be retrieved as such:
# ar -x /usr/lib/libcc_dynamic.a lib/macosx-fpsave.o

dynamic: $(SHAREDLIB)

$(SHAREDLIB): $(OFILES)
	cd blocks && $(MAKE) CFLAGS="$(CFLAGS) -I.."
	cd actioncompiler && $(MAKE) CFLAGS="$(CFLAGS) -I.."
	$(CC) $(OFILES) blocks/*.o $(COMPILEROFILES) $(LDFLAGS) $(EXTRA_LIBS) $(SHLIBLDFLAGS) -o $(SHAREDLIB)
	cp $(SHAREDLIB) ..
	ln -fs $(SHAREDLIB) ../libming$(NOVAR_SHLIBEXT)
	ln -fs $(SHAREDLIB) ../libming$(SHORT_SHLIBEXT)

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
	rm -f *.o libming.a $(SHAREDLIB) libming$(NOVAR_SHLIBEXT) test test.exe core *.core

docs::
	c2man -o../docs/man -i"ming.h" -x"SEE ALSO" -I. -D__restrict="" -D__builtin_va_list="int" $(DOCSRC)
	c2man -o../docs/libming -i"ming.h" -x"SEE ALSO" -Th -I. -D__restrict="" -D__builtin_va_list="int" $(DOCSRC)


# breaks on bsd:
#.c.o: .c .h
#	${CC} ${FLAGS} -c $<

test: static test.o
	${CC} -o test test.o libming.a  ${EXTRA_LIBS} ${LDFLAGS}
