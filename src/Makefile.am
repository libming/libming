SUBDIRS = \
	actioncompiler \
	blocks

noinst_HEADERS =	\
	actiontypes.h	\
	blocklist.h	\
	displaylist.h	\
	fill.h		\
	font_util.h	\
	gc.h		\
	libming.h	\
	ming_config.h	\
	movieclip.h	\
	movie.h		\
	position.h	\
	shape_cubic.h	\
	shape_util.h	\
	text_util.h

lib_LTLIBRARIES = libming.la

include_HEADERS = ming.h ming_config.h

libming_la_SOURCES = 	\
	blocklist.c	\
	displaylist.c	\
	fill.c		\
	font_util.c	\
	gc.c		\
	ming.c		\
	movie.c		\
	movieclip.c	\
	position.c	\
	shape_cubic.c	\
	shape_util.c	\
	text_util.c

libming_la_LDFLAGS = \
	-version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE) \
        -no-undefined 

libming_la_LIBADD = \
	actioncompiler/libactioncompiler.la \
	blocks/libblock.la \
	$(PNGLIB) \
	$(GIFLIB) \
	$(MATHLIB) \
	$(ZLIB)

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
	blocks/protect.c \
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

docs::
	c2man -o../docs/man -i"ming.h" -x"SEE ALSO" -I. -D__restrict="" -D__builtin_va_list="int" $(DOCSRC)
	c2man -o../docs/libming -i"ming.h" -x"SEE ALSO" -Th -I. -D__restrict="" -D__builtin_va_list="int" $(DOCSRC)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libming.pc

EXTRA_DIST = libming.pc.in
DISTCLEANFILES = libming.pc ming.h

# this is for the php module
all-local:
	$(mkdir_p) lib
	rm -f lib/libming.so
	$(LN_S) ../.libs/libming.so lib/libming.so

clean-local:
	rm -f lib/libming.so
	if test -d lib; then rmdir lib; fi
