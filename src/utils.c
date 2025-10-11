#include "onion.h"
#include <stddef.h>
#include <stdlib.h>

void json_destroy(json_value_t *val) {
	if (!val)
		return ;
	switch (val->type) {
		case JSON_STRING:
			free(val->val.str);
			val->val.str = NULL;
			break;
		case JSON_ARRAY:
			if (!val->val.arr)
				break;
			for (int i = 0; i < val->val.arr->len; i++)
				json_destroy(&val->val.arr->values[i]);
			free(val->val.arr->values);
			free(val->val.arr);
			val->val.arr = NULL;
			break;
		case JSON_OBJECT:
			if (!val->val.obj)
				break;
			for (int i = 0; i < val->val.obj->len; i++) {
				free(val->val.obj->keys[i]);
				json_destroy(&val->val.obj->values[i]);
			}
			free(val->val.obj->keys);
			free(val->val.obj->values);
			free(val->val.obj);
			val->val.arr = NULL;
		default:
			break;
	}
	val->type = JSON_INVALID;
}
