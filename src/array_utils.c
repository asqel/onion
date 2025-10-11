#include "onion.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int json_array_append(json_value_t *array, json_value_t *val) {
	if (!array)
		return 1;
	if (array->type != JSON_ARRAY)
		return 1;
	if (!array->val.arr)
		return 1;
	json_value_t *new_values = realloc(
		array->val.arr->values,
		(array->val.arr->len + 1) * (sizeof(json_value_t))
	);
	if (!new_values)
		return 1;
	array->val.arr->values = new_values;
	array->val.arr->values[array->val.arr->len] = *val;
	array->val.arr->len++;
	return 0;
}

int json_array_append_num(json_value_t *array, double num) {
	json_value_t val = {0};
	val.type = JSON_NUMBER;
	val.val.num = num;
	return json_array_append(array, &val);
}

int json_array_append_str(json_value_t *array, char *str) {
	if (!str)
		return 1;
	json_value_t val = {0};
	val.type = JSON_STRING;
	val.val.str = strdup(str);
	if (!val.val.str)
		return 1;

	int ret = json_array_append(array, &val);
	if (ret)
		free(val.val.str);
	return ret;
}

int json_array_append_null(json_value_t *array) {
	json_value_t val = {0};
	val.type = JSON_NULL;
	return json_array_append(array, &val);
}

int json_array_append_bool(json_value_t *array, int b) {
	json_value_t val = {0};
	val.type = JSON_NULL;
	val.val.b = b;
	return json_array_append(array, &val);
}
