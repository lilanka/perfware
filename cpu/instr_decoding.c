#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t u8;
typedef unsigned int u32;

static u8 buffer[22];

static void read_file(const char* fname) {
  FILE *ptr;
  ptr = fopen(fname, "rb");
  fread(buffer, sizeof(buffer), 1, ptr);
}

static void print_bits(u8 val) {
  u8 i = 7;
  while (i >= 0) {
    printf("%c", (val & (1 << i)) ? '1' : '0');
    if (i == 0) break; // tf, not stopping
    i--;
  }
}

static void print_bytes() {
  for (u32 i = 0; i < sizeof(buffer); ++i) {
    print_bits(buffer[i]);
    printf("\n");
  }
}

static int what_is_d(u8 val) {
  return val & 0x02 ? 1 : 0;
}

static int what_is_w(u8 val) {
  return val & 0x01 ? 1 : 0;
}

static void REG(u8* lower, int* w) {
	switch (*lower & 0x38) { // REG 
	case 0x0: *w == 0 ? printf(" al") : printf(" ax"); break;
	case 0x8: *w == 0 ? printf(" cl") : printf(" cx"); break;
	case 0x10: *w == 0 ? printf(" dl") : printf(" dx"); break;
	case 0x18: *w == 0 ? printf(" bl") : printf(" bx"); break;
	case 0x20: *w == 0 ? printf(" ah") : printf(" sp"); break;
	case 0x28: *w == 0 ? printf(" ch") : printf(" bp"); break;
	case 0x30: *w == 0 ? printf(" dh") : printf(" si"); break;
	default: *w == 0 ? printf(" bh") : printf(" di"); break;
	}
}

static void RM(u8* lower, int* w) {
	switch (*lower & 0xc0) { // MOD
	case 0xc0: { // 11 
		switch (*lower & 0x7) { // R/M
		case 0x0: *w == 0 ? printf(" al") : printf(" ax"); break;
		case 0x1: *w == 0 ? printf(" cl") : printf(" cx"); break;
		case 0x2: *w == 0 ? printf(" dl") : printf(" dx"); break;
		case 0x3: *w == 0 ? printf(" bl") : printf(" bx"); break;
		case 0x4: *w == 0 ? printf(" ah") : printf(" sp"); break;
		case 0x5: *w == 0 ? printf(" ch") : printf(" bp"); break;
		case 0x6: *w == 0 ? printf(" dh") : printf(" si"); break;
		default: *w == 0 ? printf(" bh") : printf(" di"); break;
		} 
		break;
	}
	case 0x0: { // 00
		break;
	}
	}
}

// Note:
// MOD Field:		00				01					10	
//						[ ... ]	[ ... + 8 ] [ ... + 16 ]

static void disassemble() {
	for (u32 byt = 0; byt < sizeof(buffer); byt += 2) {
		u8 higher = buffer[byt];
		u8 lower = buffer[byt + 1];

		int w = what_is_w(higher);
		int d = what_is_d(higher);

		switch (higher & 0xfc) { // Op
		case 0x88: printf("mov"); break;
		}

		if (d == 1) { // Load Op
			REG(&lower, &w);
			printf(",");
			RM(&lower, &w);
		} else { // Store Op
			RM(&lower, &w);
			printf(",");
			REG(&lower, &w);
		}
		printf("\n");
	} 
}

int main(int argc, char *argv[]) {
  read_file(argv[1]);
  disassemble();
}
