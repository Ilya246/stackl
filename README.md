# What is this?
a tiny impractical esoteric programming language i made because i was bored

## How to run this?
on linux:
1. get g++
2. `./build.sh`
3. `./out/stackl`

on windows:
1. get g++
2. 'g++ stackl.cpp -o stackl.exe'
3. run stackl.exe

also run with any arguments to run as debug mode which will print out how your program runs

## How to use it?
this is a "queue-based" programming language, value literals get pushed to the back of the queue, and functions take values from the front of the queue, removing them unless otherwise specified
instructions:

`+`, `-`, `*`, `/`, `//`, `<`, `>`: mathematical operations, take 2 values and return one; front value is considered to be left-hand side

`#`: takes nothing, returns current execution position

`~`: takes 1 value, does nothing<br>

`f`: ends execution

`r`: takes 1 value, returns it without deleting it

`e`: takes 1 value and returns it, moving it to the end of the queue

`$`: takes 2 values and returns them in swapped order

`d`, `i`, `\`, `t`, `'`: value literal specifiers: double, integer, char by-value, type, char: put specified value to end of queue<br>
example: `d32.5` `i-5` `\32` `tint` `'c`

`j`: takes 1 value and jumps execution to that index

try example.sls for an example of a program that prints numbers from 10 to 1 using a jump loop
