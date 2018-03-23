sicsim: main.o cmdProc.o shell.o memory.o hash.o
	gcc -Wall -o sicsim main.o cmdProc.o shell.o memory.o hash.o

main.o: main.h main.c
	gcc -Wall -c -o main.o main.c

cmdProc.o: main.h cmdProc.c
	gcc -Wall -c -o cmdProc.o cmdProc.c

shell.o: main.h shell.c
	gcc -Wall -c -o shell.o shell.c

memory.o: main.h memory.c
	gcc -Wall -c -o memory.o memory.c

hash.o: main.h hash.c
	gcc -Wall -c -o hash.o hash.c

clean:
	rm *.o
	rm sicsim
