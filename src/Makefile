
#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall -fPIC

LIBDIR = /usr/local/lib

OFILES = movie.o displaylist.o blocklist.o position.o movieclip.o \
	 shape_util.o shape_cubic.o text_util.o fill.o ming.o

COMPILEROFILES = actioncompiler/compile.o \
		 actioncompiler/swf4compiler.tab.o \
		 actioncompiler/lex.swf4.o \
		 actioncompiler/swf5compiler.tab.o \
		 actioncompiler/lex.swf5.o \
		 actioncompiler/compileaction.o \
		 actioncompiler/assembler.o

all: dynamic

dynamic: ${OFILES}
	cd blocks && make
	cd actioncompiler && make
	${CC} ${OFILES} ${COMPILEROFILES} blocks/*.o -shared -o libming.so
	cp libming.so ..

static: ${OFILES}
	cd blocks && make
	cd actioncompiler && make
	ar r libming.a blocks/*.o ${OFILES} ${COMPILEROFILES}
	ranlib libming.a  # bsd requires this?
	cp libming.a ..

clean:
	cd blocks && make clean
	cd actioncompiler && make clean
	rm -f *.o libming.a libming.so test test.exe core *.core

# breaks on bsd:
#.c.o: .c .h
#	${CC} ${FLAGS} -c $<

test: static test.o
	${CC} -o test test.o libming.a -lm
