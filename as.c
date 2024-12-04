#include <stdio.h>

#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[])
{
	const char *filename;
	FILE *src = stdin;

	if (argc > 1) {

		filename = argv[1];
		src = fopen(filename, "r");

		if (!src) {
			fprintf(stderr, "Unable to open source file %s\n", filename);
			return 1;
		}
	}

	token *tokens = lex(src);

	if (tokens == NULL || invalid_token(tokens)) {
		fprintf(stderr, "Failed to lex source\n");
		return 1;
	}

	ast *tree = parse(tokens);

	if (tree == NULL) {
		fprintf(stderr, "Failed to parse source \n");
		return 1;
	}

	return 0;
}
