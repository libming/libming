#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

OFILES = output.o character.o shape.o morph.o fillstyle.o matrix.o font.o \
	 block.o cxform.o text.o action.o button.o \
	 outputblock.o method.o sprite.o fontinfo.o loadfont.o \
	 rect.o jpeg.o soundstream.o mp3.o textfield.o browserfont.o \
	 dbl.o linestyle.o gradient.o bitmap.o error.o input.o #sound.o

all: ${OFILES}

clean:
	rm -f *.o libswf.a test test.exe *.core core

# breaks on bsd:
#.c.o: .c .h
#	${CC} -c $<

archive:
	tar zcvf libswf.tgz *.c *.h Makefile

libswf: ${OFILES}
	ar r swfblocks.a ${OFILES}

test: libswf test.o
	${CC} test.o -o test -L. -lswf -lm

test.o: test.c ${OFILES}
