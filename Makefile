#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

PREFIX = /usr
LIBDIR = ${PREFIX}/lib
INCLUDEDIR = ${PREFIX}/include

all: dynamic

install: dynamic
	install -d ${LIBDIR}
	install -d ${INCLUDEDIR}
	install libming.so ${LIBDIR}/libming.so.0.1
	install ming.h ${INCLUDEDIR}
	install mingpp.h ${INCLUDEDIR}
	(cd ${LIBDIR} && ln -fs libming.so.0.1 libming.so.0 && ln -fs libming.so.0 libming.so)

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
