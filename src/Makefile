#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

LIBDIR = /usr/local/lib

OFILES = movie.o displaylist.o blocklist.o position.o movieclip.o \
	 shape_util.o text_util.o fill.o ming.o

COMPILEROFILES = actioncompiler/compile.o \
		 actioncompiler/compiler.tab.o \
		 actioncompiler/lex.yy.o \
		 actioncompiler/compileaction.o

all: dynamic

dynamic: ${OFILES}
	cd blocks && make
	cd actioncompiler && make
	${CC} ${OFILES} ${COMPILEROFILES} blocks/*.o -shared -fpic \
	  -o libming.so
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
	rm -f *.o libming.a test test.exe core *.core

# breaks on bsd:
#.c.o: .c .h
#	${CC} ${FLAGS} -c $<

test: static test.o
	${CC} -o test test.o libming.a -lm
