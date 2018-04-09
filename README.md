# SIC/XE Machine

## Description

This is a series of projects for the System Programmig course in the Department of Computer Science and Engineering at Sogang University, Seoul, South Korea.

The result is a implementation of SIC/XE Machine with features such as shell environment, assembler, loader and linker.


## Specification

- Allocated virtual memory: 1 MB


## Features

- [ OK ] Shell environment
- [ OK ] Assembler
- Linker
- Loader


## Allowed Commands

- h[elp] : show list and format of commands.
- d[ir] : show list of entries in current directory.
- q[uit] : exit system.
- hi[story] : show list of valid commands entered during current session.
- du[mp] [start, end] : print content of virtual memory.
- e[dit] address, value : change value at designated memory address.
- f[ill] start, end, value : batch fill an interval in address with a value.
- reset : set whole memory to 0x00.
- opcode mnemonic : lookup for the code of the opcode mnemonic.
- opcodelist : show opcode list stored in hash table.
- assemble filename : assemble specified source file to create list file and object file.
- type filename : print the content of a file on screen.
- symbol : show the symbol table of the latest successfully assembled file.


## About the Developer

Inho Kim,

Undergraduate student of Dept. of Computer Science and Engineering,

Sogang University,

Seoul, South Korea
