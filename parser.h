#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

/*

	 program ::= instruction*

	 instruction ::= lda | rts

	 lda ::= "LDA" addressing_mode
	 rts ::= "RTS"

	 addressing_mode ::= immediate | absolute | zero_page
	 immediate ::= "#" byte
	 absolute ::= word
	 zero_page ::= byte

	 byte ::= hex hex
	 word ::= hex hex hex hex

	 hex ::= [0-9a-fA-F]

*/

typedef enum {
	LDA,
	RTS
} instruction_type;

typedef enum {
	IMMEDIATE,
	ABSOLUTE,
	ZERO_PAGE,
	IMPLIED
} addressing_mode;

typedef struct ast {
	instruction_type instruction;
	addressing_mode mode;
	union {
		struct {
			unsigned char value;
		} immediate;
		struct {
			unsigned char value;
		} zero_page;
		struct {
			unsigned short value;
		} absolute;
	};
	struct ast *next;
} ast;

ast *parse(token*);
void free_ast(ast*);

#endif
