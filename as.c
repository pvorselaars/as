#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	INVALID,
	LABEL,
	MNEMONIC,
	X,
	Y,
	SYMBOL,
	CONSTANT,
	L_PARENTHESES,
	R_PARENTHESES,
	SEMICOLON,
	COMMA,
	POUND,
	DOLLAR,
	END,
	LIST
} token_type;

typedef struct token_t {
	token_type type;
	char *value;
	struct token_t *next;
} token_t;

static char *mnemonics[] = {
	"LDA",
	"RTS"
};

int num_mnemonics = sizeof(mnemonics) / sizeof(char *);

bool is_mnemonic(const char *value)
{

	for (int i = 0; i < num_mnemonics; i++) {
		if (strcmp(mnemonics[i], value) == 0)
			return true;
	}

	return false;
}

bool is_hex(char *value)
{

	while (*value != 0) {
		if (!((*value >= '0' && *value <= '9') ||
		      (*value >= 'a' && *value <= 'f') || (*value >= 'A' && *value <= 'F')))
			return false;

		value++;
	}

	return true;
}

token_t *get_token(char **input)
{
	token_t *token = malloc(sizeof(token_t));

	if (!token) {
		fprintf(stderr, "Unable to allocate token\n");
		return NULL;
	}

	token->value = NULL;
	token->next = NULL;

	char *position = *input;

	while (isspace(*position)) {
		position++;
	}

	if (*position == 0) {

		token->type = END;

	} else if (isalpha(*position)) {

		int c = 0;
		char *str = position;
		while (isalpha(*position)) {
			c++;
			position++;
		}

		token->value = malloc(sizeof(char) * c + 1);
		strncpy(token->value, str, c);
		token->value[c] = '\0';

		if (is_mnemonic(token->value)) {
			token->type = MNEMONIC;
		} else if (*(position + 1) == ':') {
			token->type = LABEL;
			position++;
		} else if (strcmp(token->value, "X") == 0) {
			token->type = X;
		} else if (strcmp(token->value, "Y") == 0) {
			token->type = Y;
		} else {
			token->type = SYMBOL;
		}

	} else if (*position == '(') {
		token->type = L_PARENTHESES;
		position++;
	} else if (*position == '(') {
		token->type = L_PARENTHESES;
		position++;
	} else if (*position == ')') {
		token->type = R_PARENTHESES;
		position++;
	} else if (*position == ';') {
		token->type = SEMICOLON;
		position++;
	} else if (*position == ',') {
		token->type = COMMA;
		position++;
	} else if (*position == '#') {
		token->type = POUND;
		position++;
	} else if (*position == '$') {

		position++;

		int c = 0;
		char *str = position;
		while (isalnum(*position)) {
			c++;
			position++;
		}

		token->value = malloc(sizeof(char) * c + 1);
		strncpy(token->value, str, c);
		token->value[c] = '\0';

		if (is_hex(token->value)) {
			token->type = CONSTANT;
		} else {
			token->type = INVALID;
		}

	} else {
		token->type = INVALID;
		position++;
	}

	*input = position;
	return token;
}

void free_tokens(token_t * tokens)
{

	token_t *temp;
	while (tokens != NULL) {
		temp = tokens;
		tokens = tokens->next;

		if (temp->value != NULL) {
			free(temp->value);
		}
		free(temp);
	}
}

int lex(FILE * stream)
{
	char *line;
	char *buf = NULL;
	size_t len = 0;
	ssize_t num_bytes;

	token_t *list = NULL;
	token_t *current = list;

	while ((num_bytes = getline(&buf, &len, stream)) != -1) {
		line = buf;

		if (list) {
			current->next = get_token(&line);
			current = current->next;
		} else {
			list = get_token(&line);
			current = list;
		}

		while (current->type != END) {

			// the rest of the line is a comment
			if (current->type == SEMICOLON)
				break;

			// invalid token
			if (current->type == INVALID) {
				fprintf(stderr, "Invalid token %s\n", current->value);
				free_tokens(list);
				return 1;
			}

			printf("%d: %s\n", current->type, current->value);

			current->next = get_token(&line);
			current = current->next;
		}

	}

	free(buf);
	free_tokens(list);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;

	const char *filename = argv[1];
	FILE *src = fopen(filename, "r");

	if (!src) {
		fprintf(stderr, "Unable to open source file %s\n", filename);
		return 1;
	}

	if (lex(src) != 0) {
		fprintf(stderr, "Failed to lex source file %s\n", filename);
		return 1;
	}

	return 0;

}
