#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

DEBUG = --debug

LIBS = -lm

OFILES = compile.o listaction.o lex.yy.o compiler.tab.o compileaction.o

all: ${OFILES}

clean:
	rm -f ${OFILES} compiler compiler.exe core test main.o

test: ${OFILES} main.o
	${CC} -o test main.o compile.o listaction.o lex.yy.o \
		compiler.tab.o ${LIBS}

#.o: .c .h

compiler.tab.c: compiler.y
	bison compiler.y

compiler.tab.h: compiler.y
	bison --defines ${DEBUG} compiler.y

lex.yy.o: lex.yy.c

lex.yy.c: compiler.flex compiler.tab.h
	flex compiler.flex
