#include <stdio.h>

#include "lexer.h"

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

	token *list = lex(src);

	if (list == NULL || invalid_token(list)) {
		fprintf(stderr, "Failed to lex source file %s\n", filename);
		return 1;
	}

	return 0;
}
