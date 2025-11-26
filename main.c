#include "onion.h"
#include <stdio.h>

int main(int argc, char **argv) {
	json_value_t json;
	if (json_from_file("test.json", &json))
		return 1;
	json_print(&json, "  ", stdout);
	return 0;
}
