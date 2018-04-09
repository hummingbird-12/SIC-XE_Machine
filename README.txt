< Project 2 - SIC/XE Assembler >

0. How to execute program

* After unzipping, there should be:
- 6 *.h header files        (20161577.h, cmdProc.h, shell.h, memory.h, hash.h, assembler.h)
- 6 *.c source code files   (20161577.c, cmdProc.c, shell.c, memory.c, hash.c, assembler.c)
- Makefile
- Document.docx
- README
- opcode.txt
- 2_5.asm

* Then, type the following to execute program:
$ make
$ ./20161577.out

* To cleanup directory after execution, type:
$ make clean


1. Description

This is the second project for the System Programmig course in the Department of Computer Science and Engineering at Sogang University, Seoul, South Korea.
The implementation of the SIC/XE Machine Assembler is added to the shell environment created in first project.


2. Specification

- Allocated virtual memory: 1 MB


3. Allowed Commands

- h[elp]                    : show list and format of commands.
- d[ir]                     : show list of entries in current directory.
- q[uit]                    : exit system.
- hi[story]                 : show list of valid commands entered during current session.
- du[mp] [start, end]       : print content of virtual memory.
- e[dit] address, value     : change value at designated memory address.
- f[ill] start, end, value  : batch fill an interval in address with a value.
- reset                     : set whole memory to 0x00.
- opcode mnemonic           : lookup for the code of the opcode mnemonic.
- opcodelist                : show opcode list stored in hash table.
- assemble filename         : assemble .asm source file
- type filename             : print content of specified file
- symbol                    : show content of symbol table of the latest succesfully assembled source.


4. Other information

This system will be further improved with more features such as loader and linker.


5. About the Developer

Inho Kim, 20161577,
Undergraduate student of Dept. of Computer Science and Engineering,
Sogang University,
Seoul, South Korea
