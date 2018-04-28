20161577.out: main.o cmdProc.o shell.o memory.o hash.o assembler.o linkLoader.o linkedList.o
	gcc -Wall -g -o 20161577.out main.o cmdProc.o shell.o memory.o hash.o assembler.o linkLoader.o linkedList.o -lm
	@echo "\n>>> To execute, type ./20161577.out\n"

main.o: 20161577.h 20161577.c
	gcc -Wall -g -c -o main.o 20161577.c -lm

cmdProc.o: 20161577.h cmdProc.c
	gcc -Wall -g -c -o cmdProc.o cmdProc.c -lm

shell.o: 20161577.h shell.c
	gcc -Wall -g -c -o shell.o shell.c -lm

memory.o: 20161577.h memory.c
	gcc -Wall -g -c -o memory.o memory.c -lm

hash.o: 20161577.h hash.c
	gcc -Wall -g -c -o hash.o hash.c -lm

assembler.o: 20161577.h assembler.c
	gcc -Wall -g -c -o assembler.o assembler.c -lm

linkLoader.o: 20161577.h linkLoader.h
	gcc -Wall -g -c -o linkLoader.o linkLoader.c -lm

linkedList.o: 20161577.h linkedList.h shell.h
	gcc -Wall -g -c -o linkedList.o linkedList.c -lm

clean:
	-rm *.o
	-rm *.lst
	-rm 20161577.out
