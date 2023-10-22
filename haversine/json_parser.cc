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
	Tok_EoF
};

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

static char CurrentChar(Buffer *buffer) {
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

static Token Number(Buffer *buffer) {
	while (IsNumber(CurrentChar(buffer)) || CurrentChar(buffer) == '.') {
		Advance(buffer);
	}
	return CreateToken(Tok_Number);
}

static void HandleWhitespaces(Buffer *buffer) {
	while (true) {
		switch (CurrentChar(buffer)) {
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

	if (IsNumber(c)) return Number(buffer); 

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

static void ParseJSON(const char *fname, const u64 number_of_point_pairs) {
	Buffer input_data = ReadJSONFile(fname);
	Points points[number_of_point_pairs];

	while (true) {
		Token token = Tokenize(&input_data);

		printf("%d\n", token.type);

		if (token.type == Tok_EoF) break;
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
