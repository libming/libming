#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall -I.. -I. ${CFLAGS} #-fPIC -ggdb

include ../../config.make

OFILES = \
	action.o \
	bitmap.o \
	block.o \
	browserfont.o \
	button.o \
	character.o \
	cxform.o \
	dbl.o \
	error.o \
	fillstyle.o \
	font.o \
	fontinfo.o \
	gradient.o \
	imports.o \
	input.o \
	jpeg.o \
	linestyle.o \
	matrix.o \
	method.o \
	morph.o \
	mp3.o \
	output.o \
	outputblock.o \
	placeobject.o \
	rect.o \
	shape.o \
	sound.o \
	soundinstance.o \
	soundstream.o \
	sprite.o \
	text.o \
	textfield.o \
	utf8.o \
	videostream.o ${EXTRA_OBJS}

all: ${OFILES}

clean:
	rm -f *.o *.core core

# breaks on bsd:
#.c.o: .c .h
#	${CC} -c $<
