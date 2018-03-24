20161577.out: main.o cmdProc.o shell.o memory.o hash.o
	gcc -Wall -o 20161577.out main.o cmdProc.o shell.o memory.o hash.o

main.o: 20161577.h 20161577.c
	gcc -Wall -c -o main.o 20161577.c

cmdProc.o: 20161577.h cmdProc.c
	gcc -Wall -c -o cmdProc.o cmdProc.c

shell.o: 20161577.h shell.c
	gcc -Wall -c -o shell.o shell.c

memory.o: 20161577.h memory.c
	gcc -Wall -c -o memory.o memory.c

hash.o: 20161577.h hash.c
	gcc -Wall -c -o hash.o hash.c

clean:
	rm *.o
	rm 20161577.out
