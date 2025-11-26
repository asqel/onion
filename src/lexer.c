#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int append_token(token_t  **tokens, int *len,  int type, int line) {
	token_t *new = realloc(*tokens, sizeof(token_t) * (*len + 1));
	if (!new)
		return -1;
	new[*len].type = type;
	new[*len].line = line;
	(*len)++;
	*tokens = new;
	return 0;
}

token_t *_I_lexe_json(char *text, int *len) {
	size_t p = 0;
	token_t *tokens = NULL;
	int line = 1;
	while (text[p]) {
		if (text[p] == '\n') {
			p++;
			line++;
			continue;
		}
		if (isspace(text[p])) {
			p++;
			continue;
		}
		if (text[p] == '"') {
			p++;
			int start = p;
			while (text[p]) {
				if (text[p] == '"')
					break;
				if (text[p] == '\\' && text[p + 1] == '"')
					p++;
				p++;
			}
			if (text[p] != '"') {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				fprintf(stderr, "Error: unterminated string line %d\n", line);
				return NULL;
			}
			int str_len = p - start;
			char *str = malloc(str_len + 1);
			if (!str) {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				perror("json: malloc");
				return NULL;
			}
			str[str_len] = '\0';
			strncpy(str, text + start, str_len);
			p++;
			if (append_token(&tokens, len, TOKEN_STR, line) == -1) {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				perror("json: realloc");
				return NULL;
			}
			tokens[*len - 1].val.str = str;
			continue;
		}
		if (text[p] == '-' || isdigit(text[p])) {
			char *end = NULL;
			double val = strtod(&text[p], &end);
			int diff = end - &text[p];
			if (diff == 0) {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				fprintf(stderr, "Error: invalide number on line %d\n", line);
				return NULL;
			}
			p += diff;
			if (append_token(&tokens, len, TOKEN_NUM, line) == -1) {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				perror("json: realloc");
				return NULL;
			}
			tokens[*len - 1].val.num = val;
			continue;

		}
		char *keywords[3] = {
			"false",
			"true",
			"null"
		};
		int keywords_len[3] = {5, 4, 4};
		int has_found  = 0;
		for (int i = 0; i < 3; i++) {
			if (strncmp(keywords[i], &text[p], keywords_len[i]))
				continue;
			p += keywords_len[i];
			if (append_token(&tokens, len, TOKEN_BOOL, line) == -1) {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				perror("json: realloc");
				return NULL;
			}
			tokens[*len - 1].val.c = i;
			has_found = 1;
			break;
		}
		if (has_found)
			continue;
		if (strchr("[]{},:", text[p])) {
			p++;
			if (append_token(&tokens, len, TOKEN_CHAR, line) == -1) {
				_I_json_free_tokens(tokens, *len);
				*len = 0;
				perror("json: realloc");
				return NULL;
			}
			tokens[*len -1].val.c = text[p - 1];
			continue;
		}
		fprintf(stderr, "Error: unexpected token on line %d\n", line);
		_I_json_free_tokens(tokens, *len);
		*len = 0;
		return NULL;
	}
	printf("%p\n", tokens);
	return tokens;
}
