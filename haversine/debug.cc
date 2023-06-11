#pragma once

#include <cstdio>

#include "haversine.h"

void debug_tokens(Token token) {
	switch (token.type) {
	case TokenType::Left_Brace: printf("[ { ]\n"); break;
	case TokenType::Right_Brace: printf("[ } ]\n"); break;
	case TokenType::Left_Bracket: printf("[ [ ]\n"); break;
	case TokenType::Right_Bracket: printf("[ ] ]\n"); break;
	case TokenType::Comma: printf("[ , ]\n"); break;
	case TokenType::Colon: printf("[ : ]\n"); break;
	case TokenType::Minus: printf("[ - ]\n"); break;
	case TokenType::EoF: printf("[ EoF ]\n"); break;
	case TokenType::Invalid: printf("[ Invalid ]\n"); break;
	case TokenType::Number: printf("[ Number ]\n"); break;
	case TokenType::String: printf("[ String ]\n"); break;
	}
}
