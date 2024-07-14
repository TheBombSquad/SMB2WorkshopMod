
# ppc-inject

## Wat?
This is basically an assembler for the PowerPC that injects your code into a file. Note that not all instructions and mnemonics are implemented; I will add instructions that I need for my various projects. Feel free to request instructions and to make pull requests. 

This is written in Haskell, but I don't think that it will be hard to add new instructions.

## Compiling

You need stack to compile the code. You could get Haskell Platform to get it
Run
```
stack build
```
to compile

## How to use?
Read DOCUMENTATION.txt for usage and documentation. Also, look at my bagel.asm to see an example of an assembly file

## State of the project
Most of the important instructions are in here, but not all. The supported instructions are listed in DOCUMENTATION.txt

