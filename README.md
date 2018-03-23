# Proj1_SIC_Shell

## Description

This is the first project for the System Programmig course in the Department of Computer Science and Engineering at Sogang University, Seoul, South Korea.

It is a simple shell system made with C.

You may play around with different commands available, have a look at how memory is managed by a system and learn about simple opcodes.


## Specification

- Allocated virtual memory: 1 MB


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


## Other information

This system will further be improved with more features such as assembler, etc.


## About the Developer

Inho Kim,

Undergraduate student of Dept. of Computer Science and Engineering,

Sogang University,

Seoul, South Korea
