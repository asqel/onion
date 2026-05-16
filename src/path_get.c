#include "onion.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

json_value_t *json_vpath_get(json_value_t *obj, char **keys) {
	json_value_t *current = obj;

	while (*keys && current) {
		if (current->type == JSON_ARRAY) {
			int idx = atoi(*keys);
			current = json_array_get(current, idx);
		}
		else if (current->type == JSON_OBJECT)
			current = json_object_get(current, *keys);
		else {
			return NULL;
		}
		keys++;
	}

	if (current == obj)
		return NULL;
	return current;
}

json_value_t *json_path_get(json_value_t *obj, ...) {
	va_list counter_list;
	va_list lst;
	va_start(counter_list, obj);
	va_start(lst, obj);

	size_t count = 0;
	while (1) {
		char *arg = va_arg(counter_list, char *);
		if (!arg)
			break;
		count++;
	}
	va_end(counter_list);

	char **path = malloc(sizeof(char **) * (count + 1));
	if (!path) {
		va_end(lst);
		return NULL;
	}
	count = 0;
	while (1) {
		char *arg = va_arg(lst, char *);
		if (!arg)
			break;
		path[count] = arg;
		count++;
	}
	path[count] = NULL;

	json_value_t *ret = json_vpath_get(obj, path);
	free(path);
	return ret;
}
