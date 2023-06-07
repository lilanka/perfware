#include <cstdio>
#include <cstdint>
#include <fstream>

typedef uint8_t u8;
typedef uint32_t u32;

struct Memory {
	u8 bytes[1024*1024];
};

enum class OpCode {
	Mov,
	Add
};

enum class Reg {
	
};

struct Instruction {
	OpCode op;
};

#define Array_Count(array) (sizeof(array) / sizeof((array)[0]))

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

static void disassemble_8086(Memory *memory, u32 *bytes_read) {
	u8 at = 0;

	// Loop through the bytes
	while (at < *bytes_read) {
		Instruction *instruction = {};

		printf("|");
		print_bits(memory->bytes[at]);
		printf("|");
		print_bits(memory->bytes[at+1]);
		printf("|\n");
		at += 2;
	}
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
