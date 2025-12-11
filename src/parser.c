#include "parser.h"
#include "onion.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//static void print_token(token_t *tokens, int len) {
//	for (int i = 0; i < len; i++) {
//		if (tokens[i].type == TOKEN_NUM) {
//			if ((int)tokens[i].val.num == tokens[i].val.num)
//				printf("%d ", (int)tokens[i].val.num);
//			else
//				printf("%f ", tokens[i].val.num);
//		}
//		else if (tokens[i].type == TOKEN_STR)
//			printf("\"%s\" ", tokens[i].val.str);
//		else if (tokens[i].type == TOKEN_BOOL)
//			printf("%s ", (char *[]){"false", "true", "null"}[(int)tokens[i].val.c]);
//		else if (tokens[i].type == TOKEN_CHAR)
//			printf("%c ", tokens[i].val.c);
//	}
//	printf("\n");
//}

static int find_bracket(token_t *tokens, int len, int start, char left, char right) {
	int count = 0;

	for (int i = start + 1; i < len; i++) {
		if (count == 0 && tokens[i].type == TOKEN_CHAR && tokens[i].val.c == right)
			return i;
		if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == right)
			count--;
		else if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == left)
			count++;
	}
	return -1;
}

static json_value_t parse_array(token_t *tokens, int len);

static json_value_t parse_object(token_t *tokens, int len) {
	json_value_t res = {0};
	res.type = JSON_OBJECT;
	res.val.obj = malloc(sizeof(json_object_t));
	if (!res.val.obj) {
		perror("malloc");
		res.type = JSON_INVALID;
		return res;
	}
	res.val.obj->len = 0;
	res.val.obj->values = NULL;
	res.val.obj->keys = malloc(sizeof(char *));
	if (!res.val.obj->keys) {
		perror("malloc");
		free(res.val.obj);
		res.type = JSON_INVALID;
		return res;
	}
	res.val.obj->keys[0] = NULL;
	if (len == 0)
		return res;
	int need_comma = 0;
	int i = 0;
	while (i < len) {
		if (need_comma) {
			if (tokens[i].type != TOKEN_CHAR || tokens[i].val.c != ',') {
				fprintf(stderr, "Error: expected ',' on line %d\n", tokens[i].line);
				json_destroy(&res);
				break;
			}
			need_comma = 0;
			i++;
			continue;
		}
		if (tokens[i].type != TOKEN_STR) {
			fprintf(stderr, "Error: expected key string on line %d\n", tokens[i].line);
			json_destroy(&res);
			break;
		}
		char *key = tokens[i].val.str;
		i++;
		if (i >= len || !(tokens[i].type == TOKEN_CHAR && tokens[i].val.c == ':')) {
			fprintf(stderr, "Error: expected ':' after key string on line %d\n", tokens[i - 1].line);
			json_destroy(&res);
			break;
		}
		i++;
		if (i >= len) {
			fprintf(stderr, "Error: expected value ':' on line %d\n", tokens[i - 1].line);
			json_destroy(&res);
			break;
		}
		if (tokens[i].type == TOKEN_NUM) {
			int err = 0;
			if ((err = json_object_add_num(&res, key, tokens[i].val.num, 2))) {
				if (err == 2)
					fprintf(stderr, "Error: duplicate key %s on line %d\n", key, tokens[i].line);
				json_destroy(&res);
				break;
			}
			i++;
		}
		else if (tokens[i].type == TOKEN_STR) {
			int err = 0;
			if ((err = json_object_add_str(&res, key, tokens[i].val.str, 2))) {
				if (err == 2)
					fprintf(stderr, "Error: duplicate key %s on line %d\n", key, tokens[i].line);
				json_destroy(&res);
				break;
			}
			i++;
		}
		else if (tokens[i].type == TOKEN_BOOL) {
			if (tokens[i].val.c == 2) {
				int err = 0;
				if ((err = json_object_add_null(&res, key, 2))) {
					if (err == 2)
						fprintf(stderr, "Error: duplicate key %s on line %d\n", key, tokens[i].line);
					json_destroy(&res);
					break;
				}
			}
			else {
				int err = 0;
				if ((err = json_object_add_bool(&res, key, tokens[i].val.c, 2))) {
					if (err == 2)
						fprintf(stderr, "Error: duplicate key %s on line %d\n", key, tokens[i].line);
					json_destroy(&res);
					break;
				}
			}
			i++;
		}
		else if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == '{') {
			int right = find_bracket(tokens, len, i, '{', '}');
			if (right == -1) {
				fprintf(stderr, "Error: missing closing '}' on line %d\n", tokens[i].line);
				json_destroy(&res);
				break;
			}
			json_value_t obj = parse_object(&tokens[i + 1], right - i - 1);
			if (obj.type == JSON_INVALID) {
				json_destroy(&res);
				break;
			}
			int err = 0;
			if ((err = json_object_add(&res, key, &obj, 2))) {
				if (err == 2)
					fprintf(stderr, "Error: duplicate key %s on line %d\n", key, tokens[i].line);
				json_destroy(&res);
				json_destroy(&obj);
				break;
			}
			i = right + 1;
		}
		else if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == '[') {
			int right = find_bracket(tokens, len, i, '[', ']');
			if (right == -1) {
				fprintf(stderr, "Error: missing closing ']' on line %d\n", tokens[i].line);
				json_destroy(&res);
				break;
			}
			json_value_t obj = parse_array(&tokens[i + 1], right - i - 1);
			if (obj.type == JSON_INVALID) {
				json_destroy(&res);
				break;
			}
			int err = 0;
			if ((err = json_object_add(&res, key, &obj, 2))) {
				if (err == 2)
					fprintf(stderr, "Error: duplicate key %s on line %d\n", key, tokens[i].line);
				json_destroy(&res);
				json_destroy(&obj);
				break;
			}
			i = right + 1;
		}
		else {
			fprintf(stderr, "Error: unexpected token on line %d\n", tokens[i].line);
			json_destroy(&res);
			break;
		}
		need_comma = 1;
	}
	if (!need_comma && res.type != JSON_INVALID) {
		fprintf(stderr, "Error: object cannot end with trailling ',' on line %d\n", tokens[len - 1].line);
		json_destroy(&res);
	}
	return res;
}


static json_value_t parse_array(token_t *tokens, int len) {
	json_value_t res = {0};
	res.type = JSON_ARRAY;
	res.val.arr = malloc(sizeof(json_array_t));
	if (!res.val.arr) {
		res.type = JSON_INVALID;
		return res;
	}
	res.val.arr->len = 0;
	res.val.arr->values = NULL;
	if (len == 0)
		return res;
	int need_comma = 0;
	int i = 0;
	while (i < len) {
		if (need_comma) {
			if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == ',')
				need_comma = 0;
			else {
				fprintf(stderr, "Error: expected ',' on line %d\n", tokens[i].line);
				json_destroy(&res);
				break;
			}
			i++;
			continue;
		}
		if (tokens[i].type == TOKEN_NUM) {
			if (json_array_append_num(&res, tokens[i].val.num)) {
				json_destroy(&res);
				break;
			}
			i++;
		}
		else if (tokens[i].type == TOKEN_STR) {
			if (json_array_append_str(&res, tokens[i].val.str)) {
				json_destroy(&res);
				break;
			}
			i++;
		}
		else if (tokens[i].type == TOKEN_BOOL) {
			if (tokens[i].val.c == 2) {
				if (json_array_append_null(&res)) {
					json_destroy(&res);
					break;
				}
			}
			else {
				if (json_array_append_bool(&res, tokens[i].val.c)) {
					json_destroy(&res);
					break;
				}
			}
			i++;
		}
		else if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == '{') {
			int right = find_bracket(tokens, len, i, '{', '}');
			if (right == -1) {
				fprintf(stderr, "Error: missing closing '}' on line %d\n", tokens[i].line);
				json_destroy(&res);
				break;
			}
			json_value_t obj = parse_object(&tokens[i + 1], right - i - 1);
			if (obj.type == JSON_INVALID) {
				json_destroy(&res);
				break;
			}
			if (json_array_append(&res, &obj)) {
				json_destroy(&res);
				json_destroy(&obj);
				break;
			}
			i = right + 1;
		}
		else if (tokens[i].type == TOKEN_CHAR && tokens[i].val.c == '[') {
			int right = find_bracket(tokens, len, i, '[', ']');
			if (right == -1) {
				fprintf(stderr, "Error: missing closing ']' on line %d\n", tokens[i].line);
				json_destroy(&res);
				break;
			}
			json_value_t obj = parse_array(&tokens[i + 1], right - i - 1);
			if (obj.type == JSON_INVALID) {
				json_destroy(&res);
				break;
			}
			if (json_array_append(&res, &obj)) {
				json_destroy(&res);
				json_destroy(&obj);
				break;
			}
			i = right + 1;
		}
		else {
			fprintf(stderr, "Error: unexpected token on line %d\n", tokens[i].line);
			json_destroy(&res);
			break;
		}
		need_comma = 1;
	}
	if (!need_comma && res.type != JSON_INVALID) {
		json_destroy(&res);
		fprintf(stderr, "Error: array cannot end with trailling ',' on line %d\n", tokens[len - 1].line);
	}
	return res;
}

json_value_t *_I_json_parse(char *text) {
	int len = 0;
	token_t *tokens = _I_lexe_json(text, &len);
	if (!tokens)
		return NULL;
	if (len < 2) {
		fprintf(stderr, "Error: unexpected token on line %d\n", len ? tokens[0].line : 1);
		_I_json_free_tokens(tokens, len);
		return NULL;
	}
	if (tokens[0].type == TOKEN_CHAR && tokens[0].val.c == '[') {
		if (tokens[len - 1].type != TOKEN_CHAR || tokens[len - 1].val.c != ']') {
			fprintf(stderr, "Error: expected ']' as last token\n");
			_I_json_free_tokens(tokens, len);
			return NULL;
		}
		json_value_t *res = malloc(sizeof(json_value_t));
		if (!res) {
			perror("malloc");
			_I_json_free_tokens(tokens, len);
			return NULL;
		}
		res->type = JSON_INVALID;
		*res = parse_array(&tokens[1], len - 2);
		if (res->type == JSON_INVALID) {
			free(res);
			_I_json_free_tokens(tokens, len);
			return NULL;
		}
		_I_json_free_tokens(tokens, len);
		return res;
	}
	else if (tokens[0].type == TOKEN_CHAR && tokens[0].val.c == '{') {
		if (tokens[len - 1].type != TOKEN_CHAR || tokens[len - 1].val.c != '}') {
			fprintf(stderr, "Error: expected '}' as last token\n");
			_I_json_free_tokens(tokens, len);
			return NULL;
		}
		json_value_t *res = malloc(sizeof(json_value_t));
		if (!res) {
			perror("malloc");
			_I_json_free_tokens(tokens, len);
			return NULL;
		}
		res->type = JSON_INVALID;
		*res = parse_object(&tokens[1], len - 2);
		if (res->type == JSON_INVALID) {
			free(res);
			_I_json_free_tokens(tokens, len);
			return NULL;
		}
		_I_json_free_tokens(tokens, len);
		return res;
	
	}
	fprintf(stderr, "Error: unexpected token on line %d\n", tokens[0].line);
	_I_json_free_tokens(tokens, len);
	return NULL;
}
