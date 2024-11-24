#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE       32

#define SYMBOL_DEFINED    1
#define SYMBOL_UNDEFINED  0
#define SYMBOL_SIZE_MAX   16

#define OPCODE_TABLE_SIZE 70

#define WORD_SIZE         0xff

void fatal(char *msg)
{
	if (msg)
		fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

bool is_label(char *token)
{
	if (isdigit(*token))
		return false;

	while (*token != '\0') {
		if (*token == ':') {
			*token = '\0';
			return true;
		}

		token++;
	}

	return false;
}

bool is_symbol(char *token)
{
	if (!isdigit(*token))
		return true;

	return false;
}

bool is_comment(char *token)
{
	return *token == ';' ? true : false;
}

typedef struct {
	char symbol[SYMBOL_SIZE_MAX + 1];
	unsigned int value;
	char type;
} symbol_t;

symbol_t symbol_table[32];

static unsigned int symbols = 0;

symbol_t *get_symbol(char *symbol)
{
	for (int i = 0; i < symbols; i++) {
		if (strcmp(symbol, symbol_table[i].symbol) == 0)
			return &symbol_table[i];
	}
	return NULL;
}

symbol_t *create_symbol(char *symbol, unsigned int value, char type)
{
	int n = snprintf(symbol_table[symbols].symbol, SYMBOL_SIZE_MAX, "%s", symbol);
	if (n == -1)
		fprintf(stderr, "%s label too large for symbol table\n", symbol);

	symbol_table[symbols].value = value;
	symbol_table[symbols].type = type;

	return &symbol_table[symbols++];
}

typedef struct {
	char mnemonic[5];
	char opcode;
	char mode;
	char size;
} op_t;

enum {
	MODE_ABSOLUTE,
	MODE_ABSOLUTE_INDEXED_INDIRECT,
	MODE_ABSOLUTE_INDEXED_X,
	MODE_ABSOLUTE_INDEXED_Y,
	MODE_ABSOLUTE_INDIRECT,
	MODE_ACCUMULATOR,
	MODE_IMMEDIATE,
	MODE_IMPLIED,
	MODE_PC_RELATIVE,
	MODE_STACK,
	MODE_ZERO_PAGE,
	MODE_ZERO_PAGE_INDEXED_INDIRECT,
	MODE_ZERO_PAGE_INDEXED_X,
	MODE_ZERO_PAGE_INDEXED_Y,
	MODE_ZERO_PAGE_INDIRECT
};

op_t opcode_table[OPCODE_TABLE_SIZE] = {
	{"BRK", 0x00, MODE_STACK, 1},
	{"ORA", 0x01, MODE_ZERO_PAGE_INDIRECT, 2},
	{"TSB", 0x04, MODE_ZERO_PAGE, 2},
	{"ORA", 0x05, MODE_ZERO_PAGE, 2},
	{"ASL", 0x06, MODE_ZERO_PAGE, 2},
	{"RMB0", 0x07, MODE_ZERO_PAGE, 2},
	{"PHP", 0x08, MODE_STACK, 1},
	{"ORA", 0x09, MODE_IMMEDIATE, 2}
};

bool is_mnemonic(char *token)
{

	for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
		if (strcmp(opcode_table[i].mnemonic, token) == 0)
			return true;
	}

	return false;
}

op_t *get_op(char *token, char mode)
{

	for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
		if (strcmp(opcode_table[i].mnemonic, token) == 0 && opcode_table[i].mode == mode)
			return &opcode_table[i];
	}

	return NULL;
}

unsigned int assemble(unsigned int line, char *token, char *code)
{
	op_t *op;
	long x;
	int bytes = 0;

	// token is not an actual operation
	if (!is_mnemonic(token)) {
		fprintf(stderr, "%d: unknown operation %s\n", line, token);
		return 0;
	}

	// determine addressing mode by getting arguments
	for (char *arg = strtok(NULL, " \t\n"); arg != NULL; arg = strtok(NULL, " \t\n")) {

		if (is_comment(arg))
			break;

		// immediate mode
		if (*arg == '#') {
			if (*++arg != '$') {
				fprintf(stderr, "%d: %s invalid address\n", line, arg);
			} else {
				x = strtol(++arg, NULL, 16);
				if (x > WORD_SIZE) {
					fprintf(stderr, "%d: %lx argument overflow\n", line, x);
					return 0;
				}

				op = get_op(token, MODE_IMMEDIATE);
				if (op) {
					code[bytes++] = op->opcode;
					code[bytes++] = x & WORD_SIZE;
					return bytes;
				} else {
					fprintf(stderr, "%d: invalid addressing mode for %s\n", line, token);
					return 0;
				}
			}
		}

	}

	return bytes;

}

int main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE], code[BUFFER_SIZE];

	char *delim = "\t \n";

	unsigned int ln, lc, b = 0;

	symbol_t *sym;

	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {

		// Increase the source line number
		ln++;

		// split the buffer according to the delimiters
		// and process each token
		for (char *token = strtok(buffer, delim); token != NULL; token = strtok(NULL, delim)) {

			if (is_comment(token))
				break;

			if (is_label(token)) {
				// If the token is a label, search the symbol table.
				// Add the label as a symbol if it does not exist
				// or update undefined symbols. If the symbol is 
				// already defined; throw an error but continue

				sym = get_symbol(token);

				if (sym) {

					if (sym->type == SYMBOL_UNDEFINED) {

						fprintf(stderr, "%d: label %s is doubly defined.\n", ln, token);

					} else {

						sym->type = SYMBOL_DEFINED;
						sym->value = lc;

					}

				} else {
					create_symbol(token, lc, SYMBOL_DEFINED);
				}

				continue;
			}

			b = assemble(ln, token, code);

			fwrite(&code, sizeof(char), b, stdout);

			lc += b;

			continue;
		}
	}

	exit(EXIT_SUCCESS);
}
