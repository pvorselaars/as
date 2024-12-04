#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	INVALID,
	LABEL,
	KEYWORD,
	IDENTIFIER,
	CONSTANT,
	L_PARENTHESES,
	R_PARENTHESES,
	COMMA,
	POUND,
	DOLLAR
} token_type;

typedef struct token {
	token_type type;
	char *value;
	struct token *next;
} token;

token *lex(FILE*);
bool invalid_token(token*);
void free_tokens(token*);
token *take_token(token*);
void discard_token(token**);

#endif
