#include "onion.h"
#include <stdio.h>

static void print_indent(char *indent, int level, FILE *f) {
	while (level--)
		fputs(indent, f);
}


static void print_rec(json_value_t *json, char *indent, int level, FILE *f) {
	if  (!json)
		return ;
	if (json->type == JSON_NUMBER) {
		print_indent(indent, level, f);
		fprintf(f, "%f", json->val.num);
		return ;
	}
	if (json->type == JSON_STRING) {
		print_indent(indent, level, f);
		fprintf(f, "\"%s\"", json->val.str);
		return ;
	}
	if (json->type == JSON_BOOL) {
		print_indent(indent, level, f);
		fprintf(f, "%s", json->val.b ? "true" : "false");
		return ;
	}
	if (json->type == JSON_NULL) {
		print_indent(indent, level, f);
		fputs("null", f);
		return ;
	}
	if (json->type == JSON_OBJECT) {
		for (int i = 0; i < json->val.obj->len; i++) {
			char *key = json->val.obj->keys[i];
			json_value_t *val = &json->val.obj->values[i];
			print_indent(indent, level, f);
			fprintf(f, "\"%s\": ", key);
			switch (val->type) {
				case JSON_NUMBER:
				case JSON_STRING:
				case JSON_BOOL:
				case JSON_NULL:
					print_rec(val, "", 0, f);
					break;
				case JSON_OBJECT:
					fputs("{\n", f);
					print_rec(val, indent, level + 1, f);
					print_indent(indent, level, f);
					fputc('}', f);
					break;
				case JSON_ARRAY:
					fputs("[\n", f);
					print_rec(val, indent, level + 1, f);
					print_indent(indent, level, f);
					fputc(']', f);
					break;
				default:
					break;
			}
			if (i != json->val.obj->len - 1)
				fputc(',', f);
			fputc('\n', f);
		}
	}
	if (json->type == JSON_ARRAY) {
		for (int i = 0; i < json->val.arr->len; i++) {
			json_value_t *val = &json->val.arr->values[i];
			print_indent(indent, level, f);
			switch (val->type) {
				case JSON_NUMBER:
				case JSON_STRING:
				case JSON_BOOL:
				case JSON_NULL:
					print_rec(val, "", 0, f);
					break;
				case JSON_OBJECT:
					fputs("{\n", f);
					print_rec(val, indent, level + 1, f);
					print_indent(indent, level, f);
					fputc('}', f);
					break;
				case JSON_ARRAY:
					fputs("[\n", f);
					print_rec(val, indent, level + 1, f);
					print_indent(indent, level, f);
					fputc(']', f);
					break;
				default:
					break;
			}
			if (i != json->val.arr->len - 1)
				fputc(',', f);
			fputc('\n', f);
		}
	}
}

void json_print(json_value_t *json, char *indent, FILE *f) {
	if (!json)
		return ;
	if (json->type == JSON_ARRAY) {
		fputs("[\n", f);
		print_rec(json, indent, 1, f);
		fputs("]\n", f);
	}
	if (json->type == JSON_OBJECT) {
		fputs("{\n", f);
		print_rec(json, indent, 1, f);
		fputs("}\n", f);
	}
}
