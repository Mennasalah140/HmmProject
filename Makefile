all: libmyhmm.a libmyhmm.so

libmyhmm.a: myhmm.o
	ar rcs libmyhmm.a myhmm.o

libmyhmm.so: myhmm2.o
	gcc -shared -o libmyhmm.so myhmm.o

myhmm.o: myhmm.c
	gcc -c myhmm.c

myhmm2.o: myhmm.c
	gcc -c -g -fPIC myhmm.c -o myhmm.o
