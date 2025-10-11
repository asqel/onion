#include "parser.h"
#include <stdlib.h>

void _I_json_free_tokens(token_t *tokens, size_t len) {
	while(len--) {
		if (tokens[len].type == TOKEN_STR)
			free(tokens[len].val.str);
	}
	free(tokens);
}
