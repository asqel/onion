#ifndef ONION_H
#define ONION_H

#include <stdint.h>

enum {
	JSON_INVALID,
	JSON_NUMBER,
	JSON_STRING,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_BOOL,
	JSON_NULL
};

struct json_object_t {
	int len;
	char **keys; // NULL terminated
	struct json_value_t *values;
};

struct json_array_t {
	int len;
	struct json_value_t *values;
};

struct json_value_t {
	unsigned char type;
	union {
		double num;
		char *str;
		struct json_object_t *obj;
		struct json_array_t *arr;
		unsigned char b;
	} val;
};

#define JSON_CHECK_TYPE(X, TYPE) (X && X->type == TYPE)

typedef struct json_value_t json_value_t;
typedef struct json_object_t json_object_t;
typedef struct json_array_t json_array_t;

void json_destroy(json_value_t *val); // does not free val

//returns 1 on error
int json_array_append(json_value_t *array, json_value_t *val); // deos not duplicate val
int json_array_append_num(json_value_t *array, double num);
int json_array_append_str(json_value_t *array, char *str);
int json_array_append_null(json_value_t *array);
int json_array_append_bool(json_value_t *array, int b);

int json_object_add(json_value_t *obj, char *key, json_value_t *val, int replace_err); // does not duplicate val
int json_object_add_str(json_value_t *obj, char *key, char *str, int replace_err);
int json_object_add_num(json_value_t *obj, char *key, double num, int replace_err);
int json_object_add_bool(json_value_t *obj, char *key, int b, int replace_err);
int json_object_add_null(json_value_t *obj, char *key, int replace_err);

// returns 1 on error and sets res->type to JSON_INVALID
int json_from_str(char *json, json_value_t *res);
int json_from_file(char *path, json_value_t *res);

json_value_t *json_object_get(json_value_t *obj, char *key);
json_value_t *json_array_get(json_value_t *obj, int idx)

#endif
