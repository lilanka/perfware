#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <sys/stat.h>

typedef double f64;
typedef uint8_t u8;
typedef uint64_t u64;
typedef int b32;

enum TokenType {
	Tok_Number,
	Tok_Nan,
	Tok_Minus,
	Tok_Dot,
	Tok_LeftBrace,
	Tok_RightBrace,
	Tok_Comma,
	Tok_Colon,
	Tok_Invalid,
	Tok_LeftBracket,
	Tok_RightBracket,
	Tok_String,
	Tok_EoF,

	Tok_Count
};

// NOTE (lilanka): In JSON states, Group is considered as whatever inside {}, []. 
// String is considered as the whole part of 'x': <whatever here>. You know Comma
enum ParserState {
	State_Group,
	State_String,
	State_Comma,

	State_Count
};

// TODO (lilanka): Add other information (token name, line number, etc) 
struct Token {
	TokenType type;
};

struct Points {
	f64 x0, y0, x1, y1;
};

struct Buffer {
	size_t count;
	u8 *data;
};

// TODO (lilanka): Do we really need previous token? We have previous State. Look that up.
struct Parser {
	Token token;

	ParserState state;
};

static Buffer AllocateBuffer(size_t count) {
	Buffer result = {};
	result.data = (u8 *)malloc(count);
	if (result.data) {
		result.count = count;
	} else {
		fprintf(stderr, "Unable to allocate");
	}
	return result;
}

static void FreeBuffer(Buffer *buffer) {
	if (buffer->data) {
		free(buffer->data);
	}
	*buffer = {};
}

static Buffer ReadJSONFile(const char *fname) {
	Buffer result = {};
	FILE *file = fopen(fname, "r");

	if (file) {
		struct stat s;
		stat(fname, &s);

		result = AllocateBuffer(s.st_size);
		if (result.data) {
			if (fread(result.data, result.count, 1, file) != 1) {
				fprintf(stderr, "Unable to read the file");
				FreeBuffer(&result);
			}
		}
	} else {
		fprintf(stderr, "Unable to open the file");
	}
	return result;
}

static char Advance(Buffer *buffer) {
	buffer->data++;
	return buffer->data[-1];
}

static char tokenChar(Buffer *buffer) {
	return *(buffer->data);
}

static b32 IsAtEnd(Buffer *buffer) {
	return *(buffer->data) == '\0';
}

static b32 IsNumber(char c) {
	return c >= '0' && c <= '9';
}

static Token CreateToken(TokenType type) {
	Token token;
	token.type = type;
	return token;
}

static Token String(Buffer *buffer) {
	while (Advance(buffer) != '\'') {
	}
	return CreateToken(Tok_String);
}

static Token Nan(Buffer *buffer) {
	while (Advance(buffer) != ' ') {}
	return CreateToken(Tok_Nan);
}

static Token Number(Buffer *buffer) {
	while (IsNumber(tokenChar(buffer)) || tokenChar(buffer) == '.' || tokenChar(buffer) == '-') {
		Advance(buffer);
	}
	return CreateToken(Tok_Number);
}

static void HandleWhitespaces(Buffer *buffer) {
	while (true) {
		switch (tokenChar(buffer)) {
			case ' ':
			case '\r': 
			case '\t': Advance(buffer); break;
			case '\n': Advance(buffer); break; // TODO (lilanka): Add token line number increment
			default: return;	
		}
	}
}

static Token Tokenize(Buffer *buffer) {
	HandleWhitespaces(buffer);

	if (IsAtEnd(buffer)) return CreateToken(Tok_EoF);
	
	char c = Advance(buffer);

	if (IsNumber(c) || c == '-') return Number(buffer); 

	// NOTE (lilanka): Don't know if nan is allowed in JSON at all
	if (c == 'n') { return Nan(buffer); } 

	switch (c) {
		case ',': return CreateToken(Tok_Comma);
		case ':': return CreateToken(Tok_Colon);
		case '\'': return String(buffer);
		case '{': return CreateToken(Tok_LeftBrace);
		case '}': return CreateToken(Tok_RightBrace);
		case '[': return CreateToken(Tok_LeftBracket);
		case ']': return CreateToken(Tok_RightBracket);
	}

	return CreateToken(Tok_Invalid); 
}

static const char* TokenName(TokenType type) {
	switch (type) {
		case Tok_LeftBracket: return "{";
		case Tok_RightBracket: return "}";
		case Tok_LeftBrace: return "[";
		case Tok_RightBrace: return "]";
	}
}

static b32 String(Buffer *buffer, Parser *parser);
static b32 Grouping(Buffer *buffer, Parser *parser, TokenType should_be_type);

static b32 String(Buffer *buffer, Parser *parser) {
	parser->state = State_String;
	parser->token = Tokenize(buffer);
	if (parser->token.type == Tok_Colon) {
		parser->token = Tokenize(buffer);
		if (parser->token.type == Tok_Number) {
			return 1;
		} else if (parser->token.type == Tok_LeftBracket) {
			return Grouping(buffer, parser, Tok_RightBracket);
		} else if (parser->token.type == Tok_Nan) {
			return 1;
		}
	}
	return 0;
}

static b32 Comma(Buffer *buffer, Parser *parser) {
	if (parser->state == State_Group || parser->state == State_String) {
		parser->state = State_Comma;
		return 1;
	}
	parser->state = State_Comma;
	return 0;
}

static b32 Grouping(Buffer *buffer, Parser *parser, const TokenType should_be_type) {
	parser->state = State_Group;
	while (true) {
		parser->token = Tokenize(buffer);
		if (parser->token.type == should_be_type) {
			if (parser->state == State_Comma) {
				printf("A comma should't be there\n");
				return 0;
			}
			return 1;
		}
		else if (parser->token.type == Tok_String) {
			String(buffer, parser);
		} else if (parser->token.type == Tok_LeftBrace) {
			Grouping(buffer, parser, Tok_RightBrace);
		} else if (parser->token.type == Tok_Comma) {
			Comma(buffer, parser);
		} else {
			printf("There should be matching %s\n", TokenName(should_be_type));
			return 0;
		}
	}
}

static void ParseJSON(const char *fname, const u64 number_of_point_pairs) {
	Buffer input_data = ReadJSONFile(fname);
	Points points[number_of_point_pairs];
	Parser parser = {};

	while (true) {
		Token token = Tokenize(&input_data);
		
		if (token.type == Tok_LeftBrace) {
			Grouping(&input_data, &parser, Tok_RightBrace);
		} else if (token.type == Tok_EoF) break;
	}
}

int main(int argc, char **args) {
	// NOTE (lilanka): This algorithm extract number_of_point_pairs or less from the given JSON file
	const u64 number_of_point_pairs = 100000;
	const char *fname = "haversine_points.json";
	ParseJSON(fname, number_of_point_pairs);

	/* NOTE (lilanka): This is working just ignore it while building
	if (argc > 1) {
		ParseJSON(args[1]);	
	}	else {
		fprintf(stderr, "Input the JSON filename");
	}
	*/
}
