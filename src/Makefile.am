SUBDIRS = \
	actioncompiler \
	blocks

noinst_HEADERS =	\
	abctypes.h	\
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

include_HEADERS = ming.h

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
	text_util.c	\
	compat.c

libming_la_LDFLAGS = \
	-version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE) \
        -no-undefined 

libming_la_LIBADD = \
	actioncompiler/libactioncompiler.la \
	blocks/libblock.la \
	$(PNGLIB) \
	$(GIFLIB) \
	$(MATHLIB) \
	$(ZLIB) \
	$(FREETYPE_LIBS)

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
	blocks/rect.c \
	blocks/shape.c \
	blocks/sound.c \
	blocks/soundinstance.c \
	blocks/soundstream.c \
	blocks/sprite.c \
	blocks/text.c \
	blocks/textfield.c \
	blocks/utf8.c \
	blocks/videostream.c \
	blocks/filter.c \
	blocks/fileattrs.c \
	blocks/metadata.c \
	blocks/scriptlimits.c \
	blocks/scalinggrid.c \
	blocks/tabindex.c \
	blocks/symbolclass.c \
	blocks/binary.c	\
	blocks/scenedata.c \
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
	c2man -o../docs/man -i"ming.h" -x"SEE ALSO" -I. -D__USE_GNU -D__restrict="" -D__builtin_va_list="int" $(DOCSRC)
	c2man -o../docs/libming -i"ming.h" -x"SEE ALSO" -Th -I. -D__restrict="" -D__builtin_va_list="int" $(DOCSRC)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libming.pc

EXTRA_DIST = libming.pc.in
DISTCLEANFILES = libming.pc ming.h

