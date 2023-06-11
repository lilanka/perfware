#include <cstdio>
#include <cstdlib>

#include "haversine.h"
#include "debug.cc"

// * Read the input JSON file
// * Understand the JSON format (parse it)
// * Take all the data 

static u64 load_memory_from_file(const char *fname, Memory *memory) {
	u64 result = 0;
	FILE *file = fopen(fname, "r");
	if (file != nullptr) {
		result = fread(memory->buffer, 1, Array_Count(memory->buffer), file);
		fclose(file);
	}	else {
		fprintf(stderr, "Error: Unable to open %s.\n", fname);
	}
	return result;
}

static void init_scanner(Scanner *scanner, Memory *memory) {
	scanner->start = memory->buffer;
	scanner->current = memory->buffer;
	scanner->line = 0;
}

static Token create_token(TokenType type, Scanner *scanner) {
	Token token;
	token.type = type;
	token.start = scanner->start;
	token.length = (u32)(scanner->current - scanner->start);
	token.line = scanner->line;
	return token;
}

static char advance(Scanner *scanner) {
	scanner->current++;
	return scanner->current[-1];
}

static char next_char(Scanner *scanner) {
	return *scanner->current;
}

static char next_next_char(Scanner *scanner) {
	return *(scanner->current + 1);
}

static bool is_range(char c, char low, char high) {
	return c >= low && c <= high;
}

static void skip_whitespaces(Scanner *scanner) {
	while (true) {
		char c = next_char(scanner);
		switch (c) {
		case ' ':
		case '\r':
		case '\t': advance(scanner); break;
		case '\n': scanner->line++; advance(scanner); break;
		default: return;		
		}
	}
}

static Token number(Scanner *scanner) {
	while (is_range(next_char(scanner), '0', '9') || next_char(scanner) == '.')
		advance(scanner);
	return create_token(TokenType::Number, scanner);
}

static Token string(Scanner *scanner) {
	while (next_char(scanner) != '\'') advance(scanner);
	advance(scanner);
	return create_token(TokenType::String, scanner);
}

static Token tokenize(Scanner *scanner) {
	skip_whitespaces(scanner);
	scanner->start = scanner->current;

	char c = advance(scanner);

	if (is_range(c, '0', '9')) {
		return number(scanner); 
	}

	switch (c) {
	case '[': return create_token(TokenType::Left_Bracket, scanner);
	case ']': return create_token(TokenType::Right_Bracket, scanner);
	case '{': return create_token(TokenType::Left_Brace, scanner);
	case '}': return create_token(TokenType::Right_Brace, scanner);
	case ',': return create_token(TokenType::Comma, scanner);
	case ':': return create_token(TokenType::Colon, scanner);
	case '-': return create_token(TokenType::Minus, scanner);
	case '\'': return string(scanner); 
	}
	return create_token(TokenType::Invalid, scanner);
}

static ParsingResult JSON_parse(const char *fname) {
	Memory *memory = (Memory *)malloc(sizeof(Memory));
	u64 bytes_read = load_memory_from_file(fname, memory);

	Scanner scanner;
	init_scanner(&scanner, memory);

	// Tokenize
	while (true) {
		Token token = tokenize(&scanner);

		debug_tokens(token);

		if (token.type == TokenType::EoF) {
			return ParsingResult::Success;
		}
	}
	return ParsingResult::Success;
}

int main(int argc, char **args) {
	if (argc > 1) {
		ParsingResult validity = JSON_parse(args[1]);

		if (validity == ParsingResult::Success) {
			printf("Successfully parsed\n");
		} else {
			fprintf(stderr, "JSON Parsing failed\n");
		}
	} else {
		fprintf(stderr, "Input the data file\n");
	}
}
