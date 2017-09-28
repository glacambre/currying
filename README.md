# Currying in C
### Concept
#### Currying explained
Currying is the act of creating a "function" some arguments of which are
"already supplied". Take a look at the following javascript snippet:
```js
function greet (greeting, name) {
	console.log(greeting + " " + name + "!");
}

function createGreeting (greeting) {
	return function (name) {
		return greet(greeting, name);
	}
}

let hello = createGreeting("Hello");
hello("world"); // displays "Hello world!"
```
The function "greet" is wrapped inside an anonymous function that we can then
call with a single argument. Currying can be really useful in some situations.

#### Currying in C
C doesn't have obvious support for anonymous functions, which can make it look
like currying is impossible in C. However, everything is possible in C. This
currying proof of concept relies on executable memory mappings created with
mmap. We basically grab a bunch of memory, write x64 opcodes to it and return
it in order to simulate anonymous functions.

### Implementation
#### Compiling:
Any gnu/linux compiler with optimizations disabled should work. Just try
running the makefile:
```shell
make
```

#### Shortcomings of this implementation:
- x64 only.
- Only functions that take ints as parameters can be curried.
- There's a maximum of 6 arguments per function.
- Assumes SysV/Posix amd64 calling convention. Anything else will break.
- Because of the previous point, compiler optimizations will might break
  everything too.

#### Possible improvements:
- Supporting other types should be possible, the code should just be rewritten
  in order to use the XMM0...XMM7 registers instead of RDX...R9 when
  double/floats are needed.
- The limit on the maximum number of arguments could be removed by putting
  arguments into the stack.
- On an operating system that doesn't prevent a program from reading its own
  code segment, we might imagine a way to deduce the calling convention and/or
  the optimizations that happened and work around them.
