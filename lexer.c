#include "lexer.h"

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

token *get_token(char **input)
{
	int c;
	char *str, *position;
	token *t = malloc(sizeof(token));

	if (!t) {
		fprintf(stderr, "Unable to allocate token\n");
		return NULL;
	}

	t->value = NULL;
	t->next = NULL;

	position = *input;

	while (isspace(*position)) {
		position++;
	}

	if (*position == 0 || *position == ';') {

		free(t);
		return NULL;

	} else if (isalpha(*position)) {

		c = 0;
		str = position;
		while (isalnum(*position)) {
			c++;
			position++;
		}

		t->value = malloc(sizeof(char) * c + 1);
		strncpy(t->value, str, c);
		t->value[c] = '\0';

		if (is_keyword(t->value)) {
			t->type = KEYWORD;
		} else if (*(position + 1) == ':') {
			t->type = LABEL;
			position++;
		} else {
			t->type = IDENTIFIER;
		}

	} else if (*position == '(') {
		t->type = L_PARENTHESES;
		position++;
	} else if (*position == ')') {
		t->type = R_PARENTHESES;
		position++;
	} else if (*position == ',') {
		t->type = COMMA;
		position++;
	} else if (*position == '#') {
		t->type = POUND;
		position++;
	} else if (*position == '$') {

		position++;

		c = 0;
		str = position;
		while (isalnum(*position)) {
			c++;
			position++;
		}

		t->value = malloc(sizeof(char) * c + 1);
		strncpy(t->value, str, c);
		t->value[c] = '\0';

		if (is_hex(t->value)) {
			t->type = CONSTANT;
		} else {
			t->type = INVALID;
		}

	} else {
		t->type = INVALID;

		c = 0;
		str = position;
		while (isalnum(*position)) {
			c++;
			position++;
		}

		t->value = malloc(sizeof(char) * c + 1);
		strncpy(t->value, str, c);
		t->value[c] = '\0';

		position++;
	}

	*input = position;
	return t;
}

void discard_token(token ** tokens)
{
	token *temp = NULL;
	if (*tokens != NULL) {
		temp = *tokens;
		*tokens = (*tokens)->next;

		if (temp->value != NULL) {
			free(temp->value);
		}
		free(temp);
	}

	return;
}

token *take_token(token * tokens)
{
	token *temp = NULL;
	if (tokens != NULL) {
		temp = tokens;
		tokens = tokens->next;
		temp->next = NULL;
	}

	return temp;
}

void free_tokens(token * tokens)
{

	token *temp;
	while (tokens != NULL) {
		temp = tokens;
		tokens = tokens->next;

		if (temp->value != NULL) {
			free(temp->value);
		}
		free(temp);
	}
}

bool invalid_token(token * tokens)
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

token *lex(FILE * stream)
{
	char *line;
	char *buf = NULL;
	size_t len = 0;
	ssize_t num_bytes;

	token *list = NULL;
	token *prev = NULL;
	token *current = NULL;

	while ((num_bytes = getline(&buf, &len, stream)) != -1) {
		line = buf;

		while ((current = get_token(&line)) != NULL) {

			if (list == NULL)
				list = current;

			if (prev != NULL)
				prev->next = current;

			if (current->type == INVALID) {
				fprintf(stderr, "Invalid token %s\n", current->value);
			}

			prev = current;
		}

	}

	free(buf);

	return list;
}
