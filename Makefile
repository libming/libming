#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

PREFIX = /usr
LIBDIR = ${PREFIX}/lib
INCLUDEDIR = ${PREFIX}/include

all: dynamic

install: dynamic
	cp libming.so ${LIBDIR}/libming.so.0.1
	ln -sf ${LIBDIR}/libming.so.0.1 ${LIBDIR}/libming.so.0
	ln -sf ${LIBDIR}/libming.so.0 ${LIBDIR}/libming.so
	cp ming.h ${INCLUDEDIR}

dynamic:
	cd src && make dynamic

static:
	cd src && make static

clean:
	cd src && make clean
	cd util && make clean
	cd py_ext && make clean
	cd php_ext && make clean
	rm -f test.o test test.exe *.core
	rm -f libming.a libming.so

#.c.o: .c .h
#	${CC} ${FLAGS} -c $<
