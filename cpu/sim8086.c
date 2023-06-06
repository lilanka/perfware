#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef unsigned int u32;

static u8 buffer[50];

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

static int what_is_w(u8 val, char bit) {
  return val & bit ? 1 : 0;
}

// Register field encoding
static void register_field_encoding(u8* reg, int* w) {
	switch (*reg) {
	case 0x00: *w == 0 ? printf(" al") : printf(" ax"); break;
	case 0x01: *w == 0 ? printf(" cl") : printf(" cx"); break;
	case 0x02: *w == 0 ? printf(" dl") : printf(" dx"); break;
	case 0x03: *w == 0 ? printf(" bl") : printf(" bx"); break;
	case 0x04: *w == 0 ? printf(" ah") : printf(" sp"); break;
	case 0x05: *w == 0 ? printf(" ch") : printf(" bp"); break;
	case 0x06: *w == 0 ? printf(" dh") : printf(" si"); break;
	case 0x07: *w == 0 ? printf(" bh") : printf(" di"); break;
	}
}

typedef enum MOD_TYPE {
	MOD_00,
	MOD_11,
	MOD_01,
	MOD_10
} MOD_TYPE;

static MOD_TYPE RM(u8* lower, int* w) {
	switch (*lower & 0xc0) { // MOD
	case 0xc0: { // 11 
		switch (*lower & 0x7) { // R/M
		case 0x00: *w == 0 ? printf(" al") : printf(" ax"); break;
		case 0x01: *w == 0 ? printf(" cl") : printf(" cx"); break;
		case 0x02: *w == 0 ? printf(" dl") : printf(" dx"); break;
		case 0x03: *w == 0 ? printf(" bl") : printf(" bx"); break;
		case 0x04: *w == 0 ? printf(" ah") : printf(" sp"); break;
		case 0x05: *w == 0 ? printf(" ch") : printf(" bp"); break;
		case 0x06: *w == 0 ? printf(" dh") : printf(" si"); break;
		case 0x07: *w == 0 ? printf(" bh") : printf(" di"); break;
		} 
		return MOD_11;
	}
	case 0x00: { // 00
		switch (*lower & 0x7) { // R/M
		case 0x00: printf(" [bx + si]"); break;
		case 0x01: printf(" [bx + di]"); break;
		case 0x02: printf(" [bp + si]"); break;
		case 0x03: printf(" [bp + di]"); break;
		case 0x04: printf(" [si]"); break;
		case 0x05: printf(" [di]"); break; 
		case 0x06: printf(" DIRECT ADDRESS"); break;
		case 0x07: printf(" [bx]"); break;
		} 
		return MOD_00;
	}
	case 0x40: { // 01
		switch (*lower & 0x7) { // R/M
		case 0x00: printf(" [bx + si"); break;
		case 0x01: printf(" [bx + di"); break;
		case 0x02: printf(" [bp + si"); break;
		case 0x03: printf(" [bp + di"); break;
		case 0x04: printf(" [si"); break;
		case 0x05: printf(" [di"); break; 
		case 0x06: printf(" [bp]"); return MOD_11; // NOTE (Lilanka): Is this correct?
		case 0x07: printf(" [bx"); break;
		}
		return MOD_01;
	}
	case 0x80: { // 10 
		switch (*lower & 0x7) { // R/M
		case 0x00: printf(" [bx + si"); break;
		case 0x01: printf(" [bx + di"); break;
		case 0x02: printf(" [bp + si"); break;
		case 0x03: printf(" [bp + di"); break;
		case 0x04: printf(" [si"); break;
		case 0x05: printf(" [di"); break; 
		case 0x06: printf(" [bp]"); return MOD_11; // NOTE (Lilanka): Is this correct?
		case 0x07: printf(" [bx"); break;
		}
		return MOD_10;
	}
	}
}

// TODO (Lilanka):
static u8 hex_8_to_value(u8* value) {
	return 10;
} 

// TODO (Lilanka):
static u16 hex_16_to_value(u8* value1, u8* value2) {
	return 11;
}

static void disassemble() {
	u32 byt = 0;
	while (1) {
		if (byt >= sizeof(buffer)) break;

		u8 higher = buffer[byt];
		u8 lower = buffer[byt + 1];

		if ((higher & 0xf8) == 0x88) { // Register/memory to/from register
			printf("mov");

			int w = what_is_w(higher, 0x01);
			int d = what_is_d(higher);
			u8 reg = (lower & 0x38) >> 3;
			MOD_TYPE mod_type;

			if (d == 1) { // Load Op
				register_field_encoding(&reg, &w);
				printf(",");
				mod_type = RM(&lower, &w);
			} else { // Store Op
				mod_type = RM(&lower, &w);
				printf(",");
				register_field_encoding(&reg, &w);
			}

			if (mod_type == MOD_01) {
				u8 second_lower = buffer[byt + 2];
				u8 imm_value = hex_8_to_value(&second_lower);
				printf(" + %i]", imm_value);
				byt++;
			} else if (mod_type == MOD_10) {
				u8 second_lower = buffer[byt + 2];
				u8 third_lower = buffer[byt + 3];
				u8 imm_value = hex_16_to_value(&second_lower, &third_lower);
				printf(" + %i]", imm_value);
				byt += 2;
			} 
			byt++;

		} else if ((higher & 0xf0) == 0xb0) { // Immediate-to-register
			printf("mov");

			int w = what_is_w(higher, 0x08);
			u8 reg = (higher & 0x07);

			if (w == 0) { // 8-bit
				register_field_encoding(&reg, &w);
				printf(",");
				u8 imm_value = hex_8_to_value(&lower);
				printf("%i", imm_value);
				byt += 1;
			}	else { // 16-bit
				register_field_encoding(&reg, &w);
				printf(",");
				u8 second_lower = buffer[byt + 2];
				u16 imm_value = hex_16_to_value(&lower, &second_lower);
				printf("%i", imm_value);
				byt += 2;
			}
		}
		printf("\n");
		byt++;
	} 
}

int main(int argc, char *argv[]) {
  read_file(argv[1]);
  disassemble();
}
