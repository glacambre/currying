#define _DEFAULT_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

typedef enum { TYPE_INT, TYPE_DOUBLE } type;

char* as_8_bytes (char buf[8], void* addr) {
	buf[0] = ((intptr_t) addr) & 0xFF;
	buf[1] = (((intptr_t) addr) >> 8) & 0xFF;
	buf[2] = (((intptr_t) addr) >> 16) & 0xFF;
	buf[3] = (((intptr_t) addr) >> 24) & 0xFF;
	buf[4] = (((intptr_t) addr) >> 32) & 0xFF;
	buf[5] = (((intptr_t) addr) >> 40) & 0xFF;
	buf[6] = (((intptr_t) addr) >> 48) & 0xFF;
	buf[7] = (((intptr_t) addr) >> 56) & 0xFF;
	return buf + 8;
}

/* Size: 10 bytes/opcodes
 */
char* mov_rax_8bytes (char* code, void* bytes) {
	// mov rax, bytes
	*(code++) = 0x48;
	*(code++) = 0xb8;
	return as_8_bytes(code, bytes);
}

/* Size: 3 bytes
 */
char* mov_rax_byte (char* code, char byte) {
	*(code++) = 0xc6;
	*(code++) = 0x00;
	*(code++) = byte;
	return code;
}

/* Size: 4 bytes
 */
char* mov_rax1_byte (char* code, char byte) {
	*(code++) = 0xc6;
	*(code++) = 0x40;
	*(code++) = 0x01;
	*(code++) = byte;
	return code;
}

/* Size: 4 bytes
 */
char* mov_rax2_rdi (char* code) {
	*(code++) = 0x48;
	*(code++) = 0x89;
	*(code++) = 0x78;
	*(code++) = 0x02;
	return code;
}

/* Size: 1 byte
 */
char* ret (char* code) {
	// ret
	*(code++) = 0xc3;
	return code;
}

/* code: Byte array where we can write the code that should be executed when a curryied function is called
 * next_function: The function address that should be returned by the curried call
 * last_function: The last curried function than can be called, offset to where it can be written to without overwriting it
 * Size: 32 bytes
 */
char* wrap_rdi (char* code, char* next_function, char* last_function) {
	code = mov_rax_8bytes(code, last_function);

	// mov [last_function], `mov rdi, $rdi`
	code = mov_rax_byte(code, 0x48);
	code = mov_rax1_byte(code, 0xbf);
	code = mov_rax2_rdi(code);
	
	code = mov_rax_8bytes(code, next_function);
	return ret(code);
}

char* wrap_rsi (char* code, char* next_function, char* last_function) {
	code = mov_rax_8bytes(code, last_function);

	// mov [last_function], `mov rsi, $rdi`
	code = mov_rax_byte(code, 0x48);
	code = mov_rax1_byte(code, 0xbe);
	code = mov_rax2_rdi(code);

	code = mov_rax_8bytes(code, next_function);
	return ret(code);
}

char* wrap_rdx (char* code, char* next_function, char* last_function) {
	code = mov_rax_8bytes(code, last_function);
	
	// mov [last_function], `mov rdx, $rdi`
	code = mov_rax_byte(code, 0x48);
	code = mov_rax1_byte(code, 0xba);
	code = mov_rax2_rdi(code);

	code = mov_rax_8bytes(code, next_function);
	return ret(code);
}

char* wrap_rcx (char* code, char* next_function, char* last_function) {
	code = mov_rax_8bytes(code, last_function);
	
	// mov [last_function], `mov rcx, $rdi`
	code = mov_rax_byte(code, 0x48);
	code = mov_rax1_byte(code, 0xb9);
	code = mov_rax2_rdi(code);

	code = mov_rax_8bytes(code, next_function);
	return ret(code);
}

char* wrap_r8 (char* code, char* next_function, char* last_function) {
	code = mov_rax_8bytes(code, last_function);
	
	// mov [last_function], `mov r8, $rdi`
	code = mov_rax_byte(code, 0x49);
	code = mov_rax1_byte(code, 0xb8);
	code = mov_rax2_rdi(code);

	code = mov_rax_8bytes(code, next_function);
	return ret(code);
}

char* wrap_r9 (char* code, char* next_function, char* last_function) {
	code = mov_rax_8bytes(code, last_function);
	
	// mov [last_function], `mov r9, $rdi`
	code = mov_rax_byte(code, 0x49);
	code = mov_rax1_byte(code, 0xb9);
	code = mov_rax2_rdi(code);

	code = mov_rax_8bytes(code, next_function);
	return ret(code);
}

void* createfunction (void* fn, int arg_count, ...) {
	if (arg_count > 6)
		return NULL;
	// Why 512: each argument uses 32 bytes of memory and there's a maximum of 6 arguments.
	// 32 * 6 = 192, the next power of two is 256. We then double that to make sure we have enough space for the function that calls the original curried function.
	int mmap_size = 512;
	// Grab write/exec memory for the code
	char* prog = mmap(0, mmap_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	// Prog is a pointer to the place we're currently writing code to, we need prog_beginning to remember where we started
	char* prog_beginning = prog;
	// last_call is a pointer to the place withing the last function where we're writing code
	char* last_call = prog + (mmap_size / 2);
	// Since last_call moves around, we need last_call_beginning to remember where it starts
	char* last_call_beginning = last_call;
	if (prog == (void*) -1) {
		perror("mmap");
		return NULL;
	}

	// Then for each argument
	va_list arg_types;
	va_start(arg_types, arg_count);

	char* (*int_wrap[6])(char*, char*, char*) = { wrap_rdi, wrap_rsi, wrap_rdx, wrap_rcx, wrap_r8, wrap_r9 };
	for (int i = 0; i < arg_count; ++i) {
		if (va_arg(arg_types, type) == TYPE_INT) {
			// 32: Number of bytes used by an int_wrap function
			prog = int_wrap[i](prog, prog + 32, last_call);
			// 10: Number of bytes added to last_call_cur by an int_wrap function
			last_call += 10;
		} else {
			// Not supported yet
			munmap(prog, mmap_size);
			return NULL;
		}
	}

	va_end(arg_types);

	// Remove the last 'mov rax, next_function' and 'ret'.
	prog -= 11;
	// Instead 'mov r11, last_call_beginning'
	*(prog++) = 0x49;
	*(prog++) = 0xbb;
	prog = as_8_bytes(prog, last_call_beginning);
	// Then 'jmp r11'
	*(prog++) = 0x41;
	*(prog++) = 0xff;
	*(prog++) = 0xe3;

	// And in last_call, 'mov r11, fn'
	*(last_call++) = 0x49;
	*(last_call++) = 0xbb;
	last_call = as_8_bytes(last_call, fn);
	// Then 'jmp r11'
	*(last_call++) = 0x41;
	*(last_call++) = 0xff;
	*(last_call++) = 0xe3;

	return prog_beginning;
}

int times (int x, int y) {
	return x * y;
}

int add (int a, int b, int c, int d, int e, int f) {
	return a + b + c + d + e + f;
}

int main() {
	void* (*my_times)(int) = (void* (*)(int)) createfunction(times, 2, TYPE_INT, TYPE_INT);
	int (*times_two)() = (int (*)(int)) my_times(2);
	int three_times_two = times_two(3);
	printf("2 * 3 = %i\n", three_times_two);

	void* (*my_add)(int)    = (void* (*)(int)) createfunction(add, 6, TYPE_INT, TYPE_INT, TYPE_INT, TYPE_INT, TYPE_INT, TYPE_INT);
	void* (*add_one)(int)   = (void* (*)(int)) my_add(1);
	void* (*add_three)(int)   = (void* (*)(int)) add_one(2);
	void* (*add_six)(int) = (void* (*)(int)) add_three(3);
	void* (*add_ten)(int)  = (void* (*)(int)) add_six(4);
	int   (*add_fifteen)(int)  = (int (*)(int)) add_ten(5);
	printf("1 + 2 + 3 + 4 + 5 + 6 = %i\n", add_fifteen(6));
	printf("1 + 2 + 3 + 4 + 5 + 0 = %i\n", add_fifteen(0));
	return 0;
}
