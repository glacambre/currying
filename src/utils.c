#include <stdint.h>
#include "utils.h"

// Size: 8 bytes
char *as_8_bytes(char buf[8], intptr_t addr) {
	*(buf++) = addr & 0xFF;
	*(buf++) = (addr >> 8) & 0xFF;
	*(buf++) = (addr >> 16) & 0xFF;
	*(buf++) = (addr >> 24) & 0xFF;
	*(buf++) = (addr >> 32) & 0xFF;
	*(buf++) = (addr >> 40) & 0xFF;
	*(buf++) = (addr >> 48) & 0xFF;
	*(buf++) = (addr >> 56) & 0xFF;
	return buf;
}
