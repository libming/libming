#CC = gcc -g -pg -a	# profiling flags
#CC = gcc -O5
CC = gcc -g -Wall

#PREFIX = /usr/local
PREFIX = /usr
LIBDIR = ${PREFIX}/lib
INCLUDEDIR = ${PREFIX}/include

all: dynamic #static

install: install-common install-dynamic #install-static

install-common:
	install -d ${LIBDIR}
	install -d ${INCLUDEDIR}
	install ming.h ${INCLUDEDIR}
	install mingpp.h ${INCLUDEDIR}

install-dynamic: dynamic
	install libming.so ${LIBDIR}/libming.so.0.2
	(cd ${LIBDIR} && ln -fs libming.so.0.2 libming.so.0 && ln -fs libming.so.0 libming.so)

install-static: static
	install libming.a ${LIBDIR}/libming.a

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
