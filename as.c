#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
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

typedef struct token_t {
	token_type type;
	char *value;
	struct token_t *next;
} token_t;

static char *keywords[] = {
	"LDA",
	"RTS"
};

int num_keywords = sizeof(keywords) / sizeof(char *);

bool is_keyword(const char *value)
{

	for (int i = 0; i < num_keywords; i++) {
		if (strcmp(keywords[i], value) == 0)
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
	int c;
	char *str, *position;
	token_t *token = malloc(sizeof(token_t));

	if (!token) {
		fprintf(stderr, "Unable to allocate token\n");
		return NULL;
	}

	token->value = NULL;
	token->next = NULL;

	position = *input;

	while (isspace(*position)) {
		position++;
	}

	if (*position == 0 || *position == ';') {

		free(token);
		return NULL;

	} else if (isalpha(*position)) {

		c = 0;
		str = position;
		while (isalnum(*position)) {
			c++;
			position++;
		}

		token->value = malloc(sizeof(char) * c + 1);
		strncpy(token->value, str, c);
		token->value[c] = '\0';

		if (is_keyword(token->value)) {
			token->type = KEYWORD;
		} else if (*(position + 1) == ':') {
			token->type = LABEL;
			position++;
		} else {
			token->type = IDENTIFIER;
		}

	} else if (*position == '(') {
		token->type = L_PARENTHESES;
		position++;
	} else if (*position == ')') {
		token->type = R_PARENTHESES;
		position++;
	} else if (*position == ',') {
		token->type = COMMA;
		position++;
	} else if (*position == '#') {
		token->type = POUND;
		position++;
	} else if (*position == '$') {

		position++;

		c = 0;
		str = position;
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

		c = 0;
		str = position;
		while (isalnum(*position)) {
			c++;
			position++;
		}

		token->value = malloc(sizeof(char) * c + 1);
		strncpy(token->value, str, c);
		token->value[c] = '\0';

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

bool invalid_token(token_t * tokens)
{
	if (tokens == NULL)
		return false;

	while (tokens->type != INVALID) {

		if (tokens->next != NULL) {
			tokens = tokens->next;
		} else {
			return false;
		}
	}

	return true;
}

token_t *lex(FILE * stream)
{
	char *line;
	char *buf = NULL;
	size_t len = 0;
	ssize_t num_bytes;

	token_t *list = NULL;
	token_t *prev = NULL;
	token_t *current = NULL;

	while ((num_bytes = getline(&buf, &len, stream)) != -1) {
		line = buf;

		while ((current = get_token(&line)) != NULL) {

			if (list == NULL)
				list = current;

			if (prev != NULL)
				prev->next = current;

			// invalid token
			if (current->type == INVALID) {
				fprintf(stderr, "Invalid token %s\n", current->value);
			} else {
				printf("%d: %s\n", current->type, current->value);
			}

			prev = current;
		}

	}

	free(buf);

	return list;
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

	token_t *list = lex(src);

	if (list == NULL || invalid_token(list)) {
		fprintf(stderr, "Failed to lex source file %s\n", filename);
		return 1;
	}

	return 0;

}
