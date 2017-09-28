#define _DEFAULT_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <sys/syscall.h>
#include "int_regs.h"

typedef enum { TYPE_INT, TYPE_DOUBLE } type;

void* curry_int(void *fn, intptr_t param) {
	// Grab 64 bytes of executable memory
	// 64 because moving the registers is 6*3=18 bytes
	// Moving the param is 10 bytes
	// Moving the fn pointer and jumping to it is 12 bytes
	// 18+10+12 = 40 bytes, the closest power of two is 64
	char *prog = mmap(0, 64, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	char *prog_beginning = prog;

	// Write instructions of the curried function in executable memory
	// Move everything to the right
	prog = mov_r8_to_r9(prog);
	prog = mov_rcx_to_r8(prog);
	prog = mov_rdx_to_rcx(prog);
	prog = mov_rsi_to_rdx(prog);
	prog = mov_rdi_to_rsi(prog);

	// Put the argument to the curried function in rdi
	prog = mov_intptr_to_rdi(prog, param);

	// Jump to the actual function
	prog = mov_intptr_to_r11(prog, (intptr_t) fn);
	prog = jmp_to_r11(prog);

	return prog_beginning;
}

void* curry_float(void *fn, float param) {
	
}

int add(int a, int b, int c, int d, int e, int f) {
	return a + b + c + d + e + f;
}

int main() {
	int (*add_2) (int, int, int, int, int) = curry_int(add, 2);
	printf("2 + 0 + 0 + 0 + 1 = %i\n", add_2(0, 0, 0, 0, 1));
	int (*add_34) (int, int, int) = curry_int(curry_int(add_2, 5), 27);
	printf("2 + 5 + 26 + 4 + 0 + 4 = %i\n", add_34(4, 0, 4));
	return 0;
}
