#pragma once

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint16_t u16;

#define Array_Count(array) (sizeof(array) / sizeof((array)[0]))

struct Memory {
	u8 bytes[1024*1024];
};

enum class Register {
	al, ax, cl, cx, dl, dx,
	bl, bx, ah, sp, ch, bp,
	dh, si, bh, di,
	bx_si, bx_di, bp_si, bp_di
};

enum class OpCode {
	Mov,
	Add,		
};

enum class MOD_Type {
	MOD_00, MOD_11, MOD_01, MOD_10
};

// NOTE (Lilanka): This is not a good way, but it works for now
struct Instruction {
	OpCode op;
	Register reg;
	Register rm;
	MOD_Type mod_type;
	int d;
	int w;
	int imm;
};

