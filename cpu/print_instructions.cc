#include "sim8086.h"

void print_reg(Register *reg) {
	switch (*reg) {
	case Register::al: printf(" al"); break;
	case Register::ax: printf(" ax"); break;
	case Register::cl: printf(" cl"); break;
	case Register::cx: printf(" cx"); break;
	case Register::dl: printf(" dl"); break;
	case Register::dx: printf(" dx"); break;
	case Register::bl: printf(" bl"); break;
	case Register::bx: printf(" bx"); break;
	case Register::ah: printf(" ah"); break;
	case Register::sp: printf(" sp"); break;
	case Register::ch: printf(" ch"); break;
	case Register::bp: printf(" bp"); break;
	case Register::dh: printf(" dh"); break;
	case Register::si: printf(" si"); break;
	case Register::bh: printf(" bh"); break;
	case Register::di: printf(" di"); break;
	}
}

void print_rm(Register *rm) {
	// NOTE (Lilanka): This is ugly of course. Since this is for printing, no need to worry for now. Who print
	//								 these things anyway.
	switch (*rm) {
	case Register::al: printf(" al"); break;
	case Register::ax: printf(" ax"); break;
	case Register::cl: printf(" cl"); break;
	case Register::cx: printf(" cx"); break;
	case Register::dl: printf(" dl"); break;
	case Register::dx: printf(" dx"); break;
	case Register::bl: printf(" bl"); break;
	case Register::bx_si: printf(" bx + si"); break;
	case Register::bx_di: printf(" bx + di"); break;
	case Register::bp_si: printf(" bp + si"); break;
	case Register::bp_di: printf(" bp + di"); break;
	case Register::si: printf(" si"); break;
	case Register::di: printf(" di"); break;
	case Register::bx: printf(" bx"); break;
	case Register::ah: printf(" ah"); break;
	case Register::sp: printf(" sp"); break;
	case Register::ch: printf(" ch"); break;
	case Register::bp: printf(" bp"); break;
	case Register::dh: printf(" dh"); break;
	case Register::bh: printf(" bh"); break;
	}
}

void print_instructions(std::vector<Instruction> *instructions) {
	for (Instruction& instr : *instructions) {

		switch (instr.op) {
		case OpCode::Mov: printf("mov"); break;
		case OpCode::Add: printf("add"); break;
		default: printf("NaN"); break;
		}

		if (instr.d == 0) {
			print_rm(&instr.rm);
			printf(",");
			print_reg(&instr.reg);
		} else {
			print_reg(&instr.reg);
			printf(",");
			print_rm(&instr.rm);
		}
		printf("\n");
	}
}
