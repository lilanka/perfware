#include <iostream>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <vector>

#include "sim8086.h"
#include "print_instructions.cc"

static void print_bits(u8 value) {
	u8 i = 7;
	while (i >= 0) {
		printf("%c", (value & (1 << i)) ? '1' : '0');
		if (i == 0) break;
		i--;
	}
}

static u32 load_memory_from_file(char *fname, Memory *memory, u32 offset) {
	u32 result = 0;

	if (offset < Array_Count(memory->bytes)) {
		FILE *file = fopen(fname, "rb"); 
		if (file != nullptr) {
			result = fread(memory->bytes + offset, 1, Array_Count(memory->bytes) - offset, file);
			fclose(file);
		} else {
			fprintf(stderr, "Error: Unable to open %s.\n", fname);
		}
	}
	return result;
}

static int what_is_w(u8 value, char bit_value) {
	return (value & bit_value) ? 1 : 0;
}

static int what_is_d(u8 value) {
	return (value & 0x02) ? 1 : 0;
}

static void register_field_encoding(Instruction *instr, u8 *reg) {
	switch (*reg) {
	case 0x00: instr->w == 0 ? instr->reg = Register::al : instr->reg = Register::ax; break;
	case 0x01: instr->w == 0 ? instr->reg = Register::cl : instr->reg = Register::cx; break;
	case 0x02: instr->w == 0 ? instr->reg = Register::dl : instr->reg = Register::dx; break;
	case 0x03: instr->w == 0 ? instr->reg = Register::bl : instr->reg = Register::bx; break;
	case 0x04: instr->w == 0 ? instr->reg = Register::ah : instr->reg = Register::sp; break;
	case 0x05: instr->w == 0 ? instr->reg = Register::ch : instr->reg = Register::bp; break;
	case 0x06: instr->w == 0 ? instr->reg = Register::dh : instr->reg = Register::si; break;
	case 0x07: instr->w == 0 ? instr->reg = Register::bh : instr->reg = Register::di; break;
	}
}

static void RM(Instruction *instr, u8 *byt) {
	u8 mod_value = *byt & 0xc0;
	if (mod_value == 0xc0) { // 11
		switch (*byt & 0x7) { // R/M
		case 0x00: instr->w == 0 ? instr->rm = Register::al : instr->rm = Register::ax; break;
		case 0x01: instr->w == 0 ? instr->rm = Register::cl : instr->rm = Register::cx; break;
		case 0x02: instr->w == 0 ? instr->rm = Register::dl : instr->rm = Register::dx; break;
		case 0x03: instr->w == 0 ? instr->rm = Register::bl : instr->rm = Register::bx; break;
		case 0x04: instr->w == 0 ? instr->rm = Register::ah : instr->rm = Register::sp; break;
		case 0x05: instr->w == 0 ? instr->rm = Register::ch : instr->rm = Register::bp; break;
		case 0x06: instr->w == 0 ? instr->rm = Register::dh : instr->rm = Register::si; break;
		case 0x07: instr->w == 0 ? instr->rm = Register::bh : instr->rm = Register::di; break;
		}
		instr->mod_type = MOD_Type::MOD_11;
	}
	else if (mod_value == 0x00 || mod_value == 0x40 || mod_value == 0x80) {
		switch (*byt & 0x7) { // R/M
		case 0x00: instr->rm = Register::bx_si; break;
		case 0x01: instr->rm = Register::bx_di; break;
		case 0x02: instr->rm = Register::bp_si; break;
		case 0x03: instr->rm = Register::bp_di; break;
		case 0x04: instr->rm = Register::si; break;
		case 0x05: instr->rm = Register::di; break;
		case 0x06: instr->rm = Register::bp; break; // TODO (Lilanka): Direct Addressing
		case 0x07: instr->rm = Register::bx; break;
		}
		if (mod_value == 0x00) instr->mod_type = MOD_Type::MOD_00;
		else if (mod_value == 0x40) instr->mod_type = MOD_Type::MOD_01;
		else if (mod_value == 0xc0) instr->mod_type = MOD_Type::MOD_11;
		else instr->mod_type = MOD_Type::MOD_10;
	}
}

// TODO (Lilanka): 
static u8 hex_8_to_real(u8 *value) {
	return 12;
}

// TODO (Lilanka): 
static u16 hex_16_to_real(u8 *value1, u8 *value2) {
	return 13;
}

static void disassemble_8086(Memory *memory, u32 *bytes_read) {
	u8 at = 0;

	// We store all the instructions in one array
	std::vector<Instruction> instructions;

	// Loop through the bytes
	while (at < *bytes_read) {
		Instruction instr;

		u8 higher = memory->bytes[at];
		u8 lower = memory->bytes[at+1];

		printf("|");
		print_bits(higher);
		printf("|");
		print_bits(lower);
		printf("|");

		if ((higher & 0xf8) == 0x88) {
			instr.op = OpCode::Mov;
		} else if ((higher & 0xfc) == 0x00) {
			instr.op = OpCode::Add;
		}

		if (instr.op == OpCode::Mov || instr.op == OpCode::Add) {
			instr.w = what_is_w(higher, 0x01);
			instr.d = what_is_d(higher);
			u8 reg = (lower & 0x38) >> 3;

			register_field_encoding(&instr, &reg);
			RM(&instr, &lower);

			if (instr.mod_type == MOD_Type::MOD_01) {
				u8 second_lower = memory->bytes[at+2];
				u8 imm_value = hex_8_to_real(&second_lower);
				//printf(" + %i]", imm_value);
				instr.imm = imm_value;
				at++;
			} else if (instr.mod_type == MOD_Type::MOD_10) {
				u8 second_lower = memory->bytes[at+2];
				u8 third_lower = memory->bytes[at+3];
				u16 imm_value = hex_16_to_real(&second_lower, &third_lower);
				//printf(" + %i]", imm_value);
				instr.imm = imm_value;
				at+=2;
			}
			at++;
		}
		instructions.push_back(instr);
		at++;
	}

	print_instructions(&instructions);
}

int main(int argc, char **args) {
	Memory *memory = (Memory *)malloc(sizeof(Memory));

	if (argc > 1) {
		for (int idx = 1; idx < argc; ++idx) {
			char *fname = args[idx];
			u32 bytes_read = load_memory_from_file(fname, memory, 0);

			disassemble_8086(memory, &bytes_read);
		}		
	}
}
