#include <stdint.h>
#include "utils.h"
#include "int_regs.h"

// Size: 3 bytes
char *mov_r8_to_r9(char* code) {
	*(code++) = 0x4D;
	*(code++) = 0x89;
	*(code++) = 0xC1;
	return code;
}

// Size: 3 bytes
char *mov_rcx_to_r8(char* code) {
	*(code++) = 0x49;
	*(code++) = 0x89;
	*(code++) = 0xC8;
	return code;
}

// Size: 3 bytes
char *mov_rdx_to_rcx(char* code) {
	*(code++) = 0x48;
	*(code++) = 0x89;
	*(code++) = 0xD1;
	return code;
}

// Size: 3 bytes
char *mov_rsi_to_rdx(char* code) {
	*(code++) = 0x48;
	*(code++) = 0x89;
	*(code++) = 0xF2;
	return code;
}

// Size: 3 bytes
char *mov_rdi_to_rsi(char* code) {
	*(code++) = 0x48;
	*(code++) = 0x89;
	*(code++) = 0xFE;
	return code;
}

// Size: 10 bytes
char *mov_intptr_to_rdi(char* code, intptr_t param) {
	*(code++) = 0x48;
	*(code++) = 0xBF;
	code = as_8_bytes(code, param);
	return code;
}

// Size: 10 bytes
char *mov_intptr_to_r11(char *code, intptr_t param) {
	*(code++) = 0x49;
	*(code++) = 0xBB;
	code = as_8_bytes(code, param);
	return code;
}

// Size: 2 bytes
char *jmp_to_r11(char *code) {
	*(code++) = 0x41;
	*(code++) = 0xFF;
	*(code++) = 0xE3;
	return code;
}

