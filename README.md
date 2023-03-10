# PPMP
## Program Pointer | Memory Pointer
Values end up on stack when computing. Stack values can be set in and retrieved from memory.
Values in the stack and memory are [BigInts](https://github.com/rgroshanrg/bigint)

## Running 
**Optional flags**
* -DPRINT_INFO
  * Prints a "stack-trace" ***output op (MP, PP, SM) [ Memory ] | Stack***
* -SLEEP_TIME={number}
  * Sleeps the thread between each step

**Example usage**  
`g++ main.cpp -std=c++11 -O2 -o ppmp -DPRINT_INFO -DSLEEP_TIME=100`

`./ppmp examples/hello_world.ppmp`

[Math](#math)  
[Pointers](#pointers)  
[Numbers](#numbers)  
[Strings](#strings)  
[Memory](#memory)  
[Stack](#stack)

[Examples](#examples)  

### Math
| Op | Function | Pops |
|:-:|---|---|
|+|Adds the two top stack values|&check;|
|-|Subtracts the two top stack values|&check;|
|*|Multiplies the two top stack values|&check;|
|/|Divides the two top stack values|&check;|
|%|Modulus of the two top stack values|&check;|

### Pointers
| Op | Function | Pops |
|:-:|---|---|
|<|Moves the program pointer back equal to the number on the stack|&check;|
|>|Moves the program pointer forward equal to the number on the stack|&check;|
|^|Moves the memory pointer up, expands memory if needed||
|v|Moves the memory pointer down, expands memory if needed||
|j|Moves the memory pointer equal to the number at the top of the stack, expands memory if needed|&check;|
|J|Moves the memory pointer equal to the number in memory, expands memory if needed||
|e|edits the value at the program equal to value in stack % 128|&check;|
|E|edits the value at the program equal to value in memory % 128||
|]|Moves the program pointer to the corresponding closing bracket "[" if stack value is not 0||
|)|Moves the program pointer to the corresponding closing bracket "(" if memory value is not 0||

### Numbers (digits only)
| Op | Function | Pops |
|:-:|---|---|
|0 .. 9|Pushes the number to the stack||

### Strings
| Op | Function | Pops |
|:-:|---|---|
|"| Toggles _**string-mode**_ (Input read literally)||

### IO
| Op | Function | Pops |
|:-:|---|---|
|w|Print stack as char (Value % 128)|&check;|
|W|Print memory as char (Value % 128)||
|p|Print stack value as number|&check;|
|P|Print memory value as number||
|c|Read to stack as char (Value % 128)||
|C|Read to memory as char (Value % 128)||
|n|Read to stack as number ||
|N|Read to memory as number ||

## Memory
| Op | Function | Pops |
|:-:|---|---|
|g|Pushes the value in the memory cell to the top of the stack||
|s|Sets the value in the memory cell equal to the value at the top of the stack|&check;|
|d|Duplicates the value in the memory cell |&check;|

### Stack
| Op | Function | Pops |
|:-:|---|---|
|:|Duplicates the value at the top of the stack||
|$|Drops the value at the top of the stack||
|z|Swaps the top two values of the stack||

## Examples
#### Hello World!
`0"!dlroW olleH"[w]`

#### Fibonacci
`10[:p52*w:s+gz]`