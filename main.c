#include "onion.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	json_value_t json;
	if (json_from_file("test.json", &json))
		return 1;
	printf("yay\n");
	return 0;
}
