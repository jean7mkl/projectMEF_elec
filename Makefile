all: main

main: main.o avl.o io.o
	gcc -o main main.o avl.o io.o

main.o: main.c
	gcc -c main.c

avl.o: avl.c
	gcc -c avl.c

io.o: io.c
	gcc -c io.c

clean:
	rm -f *.o main

