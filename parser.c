#include "parser.h"

bool expect(token_type type, char *value, token ** tokens)
{
	if ((*tokens)->type != type || (value != NULL && strcmp(value, (*tokens)->value) != 0)) {
		return false;
	}

	return true;
}

ast *parse_instruction(token ** tokens)
{
	ast *node = malloc(sizeof(ast));

	if (expect(KEYWORD, NULL, tokens)) {

		if (expect(KEYWORD, "LDA", tokens)) {
			node->instruction = LDA;

			discard_token(tokens);

			if (expect(POUND, NULL, tokens)) {

				discard_token(tokens);

				if (expect(CONSTANT, NULL, tokens)) {

					node->mode = IMMEDIATE;
					unsigned int val = strtol((*tokens)->value, 0, 0);

					if (val > 0xff) {
						fprintf(stderr,
							"Warning, operand overflow: 0x%08x truncated to 0x%02x\n", val,
							val & 0xff);
					}
					node->immediate.value = val & 0xff;

					discard_token(tokens);
					return node;
				}

			} else if (expect(CONSTANT, NULL, tokens)) {

				unsigned int val = strtol((*tokens)->value, 0, 0);

				if (val > 0xff) {

					if (val > 0xffff) {
						fprintf(stderr,
							"Warning, operand overflow: 0x%08x truncated to 0x%02x\n", val,
							val & 0xffff);
					}

					node->mode = ABSOLUTE;
					node->absolute.value = val & 0xffff;
				} else if (val <= 0xff) {

					node->mode = ZERO_PAGE;
					node->zero_page.value = val & 0xff;
				}

				discard_token(tokens);

				return node;
			}

			fprintf(stderr, "Syntax error, unsupported addressing mode for LDA\n");

		} else if (expect(KEYWORD, "RTS", tokens)) {
			node->instruction = RTS;
			node->mode = IMPLIED;

			discard_token(tokens);

			return node;
		}

	} else {
		fprintf(stderr, "Syntax error, expected keyword\n");
	}

	exit(1);

}

ast *parse(token * tokens)
{
	ast *tree = parse_instruction(&tokens);
	ast *node = tree;

	while (tokens) {
		node->next = parse_instruction(&tokens);
		node = node->next;
	}

	return tree;
}
