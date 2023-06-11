#pragma once

#include <cstdint>

typedef uint32_t u32;
typedef uint64_t u64;
typedef double f64;

#define Array_Count(array) (sizeof(array) / sizeof((array)[0]))

struct Memory {
	char buffer[1024*1024*10]; // Our input file contains only 6Mb, but let's put 10 Mb
};

enum class ParsingResult {
	Success,
	Parsing_Error
};

enum class TokenType {
	Left_Brace,
	Right_Brace,
	Left_Bracket,
	Right_Bracket,
	Comma,
	Colon,
	Minus,
	Number,
	String,

	EoF, Invalid
};

struct Token {
	TokenType type;
	const char *start;
	u32 length;
	std::size_t line;
};

struct Scanner {
	char *start;
	char *current;
	std::size_t line;
};

struct PointPairs {
	f64 x0, y0, x1, y1;
};
