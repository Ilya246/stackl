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

### Command-line arguments
`--debug`/`-d`: run in debug mode, will print the program state every step

`--file`/`-f`: read program from file instead of console

## How to use it?
this is a "queue-based" programming language, value literals get pushed to the back/front of the queue, and functions take values from the front/back of the queue, removing them unless otherwise specified
instructions:

`+`, `-`, `*`, `/`, `//`, `%`, `<`, `>`, `=`, `!=`, `!`: mathematical operations, take 2 values and return one; front value is considered to be left-hand side

`#`: takes nothing, returns current execution position

`~`: takes 1 value, does nothing<br>

`f`: ends execution

`r`: takes 1 value, returns it without deleting it

`e`: takes 1 value and returns it, moving it to the end of the queue

`$`: takes 2 values and returns them in swapped order

`c`: takes 2 values and converts second value to type specified by first

`d`, `i`, `\`, `t`, `'`: value literal specifiers: double, integer, char by-value, type, char: put specified value to end of queue<br>
example: `d32.5` `i-5` `\32` `tint` `'c`

`j`: takes 2 values and, if second value is not 0, jumps execution to index equal to first value

any instruction may be prepended by an integer to repeat it that amount of times (example: 3e will cycle the queue by 3 elements)<br>
prepending any instruction by a minus will treat the queue in reverse (example: -3i5 will add 5 to the *front* of the queue 3 times)

try `./stackl -f filename.cls` on the examples in the `examples` folder to try it out, also use the `-d` argument to see execution flow

### How do I even code in this?

run it with -d and try things until you get what you want
