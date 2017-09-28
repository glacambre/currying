#ifndef INT_REGS_H
#define INT_REGS_H
#include <stdint.h>
char *mov_r8_to_r9(char* code);
char *mov_rcx_to_r8(char* code);
char *mov_rdx_to_rcx(char* code);
char *mov_rsi_to_rdx(char* code);
char *mov_rdi_to_rsi(char* code);
char *mov_intptr_to_rdi(char* code, intptr_t param);
char *mov_intptr_to_r11(char *code, intptr_t param);
char *jmp_to_r11(char *code);
#endif

