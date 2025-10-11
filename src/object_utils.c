#include "onion.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int json_object_add(json_value_t *obj, char *key, json_value_t *val) {
	if (!obj || !key || !val)
		return 1;
	if (obj->type != JSON_OBJECT)
		return 1;
	if (!obj->val.obj) {
		obj->val.obj = malloc(sizeof(json_object_t));
		if (!obj->val.obj)
			return 1;
		obj->val.obj->len = 0;
		obj->val.obj->keys = NULL;
		obj->val.obj->values = NULL;
	}
	for (int i = 0; i < obj->val.obj->len; i++) {
		if (!strcmp(obj->val.obj->keys[i], key)) {
			json_destroy(&obj->val.obj->values[i]);
			obj->val.obj->values[i] = *val;
			return 0;
		}
	}
	char **new_keys = realloc(
		obj->val.obj->keys,
		sizeof(char *) * (obj->val.obj->len + 2)
	);
	if (!new_keys)
		return 1;
	obj->val.obj->keys = new_keys;

	json_value_t *new_values = realloc(
		obj->val.obj->values,
		sizeof(json_value_t) * (obj->val.obj->len + 1)
	);
	if (!new_values)
		return 1;
	obj->val.obj->values = new_values;	
	key = strdup(key);
	if (!key)
		return 1;
	obj->val.obj->values = new_values;
	obj->val.obj->keys[obj->val.obj->len] = key;
	obj->val.obj->keys[obj->val.obj->len + 1] = NULL;
	obj->val.obj->values[obj->val.obj->len] = *val;
	obj->val.obj->len++;
	return 0;
}

int json_object_add_num(json_value_t *obj, char *key, double num) {
	json_value_t val = {0};
	val.type = JSON_NUMBER;
	val.val.num = num;
	return json_object_add(obj, key, &val); 
}

int json_object_add_str(json_value_t *obj, char *key, char *str) {
	if (!str)
		return 1;
	char *new_str = strdup(str);
	if (!new_str)
		return 1;
	json_value_t val = {0};
	val.type = JSON_STRING;
	val.val.str = new_str;
	int ret = json_object_add(obj, key, &val);
	if (ret)
		free(new_str);
	return ret;
}

int json_object_add_bool(json_value_t *obj, char *key, int b) {
	json_value_t val = {0};
	val.type = JSON_BOOL;
	val.val.b = b;
	return json_object_add(obj, key, &val);
}

int json_object_add_null(json_value_t *obj, char *key) {
	json_value_t val = {0};
	val.type = JSON_NULL;
	return json_object_add(obj, key, &val);
}
