
#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

DEBUG = --debug

# In case we need to get more information out of flex
#FLEXDEBUG = -d

LIBS = -lm

MOSTFILES = compile.o listaction.o \
	    lex.swf4.o lex.swf5.o swf5compiler.tab.o swf4compiler.tab.o \
	    assembler.o

GENERATED = lex.swf4.c lex.swf5.c swf5compiler.tab.c swf4compiler.tab.c

OFILES = ${MOSTFILES} compileaction.o

all: ${OFILES}

clean:
	rm -f ${OFILES} ${GENERATED} compiler compiler.exe core test main.o

test: ${MOSTFILES} main.o
	${CC} -o test main.o ${MOSTFILES} ${LIBS}

#.o: .c .h

swf5compiler.tab.c: swf5compiler.y
	bison -p swf5 swf5compiler.y

swf5compiler.tab.h: swf5compiler.y
	bison --defines ${DEBUG} -p swf5 swf5compiler.y

swf4compiler.tab.c: swf4compiler.y
	bison -p swf4 swf4compiler.y

swf4compiler.tab.h: swf4compiler.y
	bison --defines ${DEBUG} -p swf4 swf4compiler.y

lex.swf4.o: lex.swf4.c

lex.swf4.c: swf4compiler.flex swf4compiler.tab.h
	flex ${FLEXDEBUG} -i -Pswf4 swf4compiler.flex

lex.swf5.o: lex.swf5.c

lex.swf5.c: swf5compiler.flex swf5compiler.tab.h
	flex ${FLEXDEBUG} -i -Pswf5 swf5compiler.flex
