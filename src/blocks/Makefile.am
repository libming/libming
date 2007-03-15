SUBDIRS = 

INCLUDES = -I$(top_srcdir)/src

noinst_HEADERS =	\
	action.h	\
	bitmap.h	\
	block.h		\
	blocktypes.h	\
	browserfont.h	\
	button.h	\
	character.h	\
	cxform.h	\
	dbl.h		\
	error.h		\
	exports.h	\
	fillstyle.h	\
	font.h		\
	fontinfo.h	\
	fromswf.h	\
	gradient.h	\
	imports.h	\
	input.h		\
	jpeg.h		\
	libswf.h	\
	linestyle.h	\
	matrix.h	\
	method.h	\
	morph.h		\
	outputblock.h	\
	output.h	\
	placeobject.h	\
	protect.h	\
	rect.h		\
	shape.h		\
	sound.h		\
	soundinstance.h	\
	soundstream.h	\
	sprite.h	\
	swf.h		\
	textfield.h	\
	text.h		\
	ttffont.h	\
	utf8.h		\
	videostream.h	\
	filter.h	\
	flv.h		\
	fileattrs.h	\
	metadata.h	\
	scriptlimits.h	\
	scalinggrid.h	\
	tabindex.h	


noinst_LTLIBRARIES = libblock.la

if USE_PNG
PNG_SOURCE=pngdbl.c
endif

libblock_la_SOURCES = 	\
	action.c	\
	bitmap.c	\
	block.c		\
	browserfont.c	\
	button.c	\
	character.c	\
	cxform.c	\
	dbl.c		\
	error.c		\
	fillstyle.c	\
	font.c		\
	fontinfo.c	\
	fromswf.c	\
	gifdbl.c	\
	gradient.c	\
	imports.c	\
	input.c		\
	jpeg.c		\
	linestyle.c	\
	matrix.c	\
	method.c	\
	morph.c		\
	mp3.c		\
	outputblock.c	\
	output.c	\
	protect.c	\
	placeobject.c	\
	read.c		\
	rect.c		\
	shape.c		\
	sound.c		\
	soundinstance.c	\
	soundstream.c	\
	sprite.c	\
	text.c		\
	textfield.c	\
	ttffont.c	\
	utf8.c		\
	videostream.c	\
	flv.c		\
	filter.c	\
	fileattrs.c	\
	metadata.c	\
	scriptlimits.c	\
	scalinggrid.c	\
	tabindex.c	\
	$(PNG_SOURCE)

