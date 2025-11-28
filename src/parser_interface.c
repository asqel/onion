#include "onion.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

static char *read_file(const char *path, size_t *lp) {
	size_t len;

	FILE *f = fopen(path, "rb");

	if (!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (lp)
		*lp = len;

	char *res = malloc(len * sizeof(char) + 1);

	if (!res || fread(res, 1, len, f) != len) {
		free(res);
		fclose(f);
		return NULL;
	}
	res[len] = '\0';
	fclose(f);
	return res;
}

int json_from_str(char *json, json_value_t *res) {
	if (!res)
		return 1;
	if (!json) {
		res->type = JSON_INVALID;
		return 1;
	}
	json_value_t *res_ptr = _I_json_parse(json);
	if (!res_ptr) {
		res->type = JSON_INVALID;
		return 1;
	}
	*res = *res_ptr;
	free(res_ptr);
	return 0;
}

int json_from_file(char *path, json_value_t *res) {	
	if (!res)
		return 1;
	char *text = read_file(path, NULL);

	int ret = json_from_str(text, res);
	free(text);
	return ret;
}
