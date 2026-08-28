#ifndef PARSER_H
#define PARSER_H

#include "onion.h"
#include <stddef.h>

enum {
	TOKEN_INVALID,
	TOKEN_NUM,
	TOKEN_STR,
	TOKEN_BOOL,
	TOKEN_CHAR
};

typedef struct token_t {
	char type;
	union {
		double num;
		char *str;
		char c;
	} val;
	int line;
} token_t;

token_t *_I_lexe_json(char *text, int *len);
int _I_lex_number(char *str, size_t *advanced, double *res);
json_value_t *_I_json_parse(char *text);
void _I_json_free_tokens(token_t *tokens, size_t len);
int _I_lexe_escape_string(char *str, int line);

#endif
