#include "parser.h"
#include <string.h>
#include <stdio.h>

#define STATE_END 0
#define STATE_ZERO 1
#define STATE_MINUS 2
#define STATE_DIGIT 3
#define STATE_EXP 4
#define STATE_FRACT 5

static double get_10_power(double nb) {
	double val = 1;
	while (nb) {
		val *= 10;
		nb--;
	}

	return val;
}

int _I_lex_number(char *str, size_t *advanced, double *res) {
	char *start = str;
	double val = 0;
	int sign = 0;
	int state = 0;
	
	double fract = 0;
	double fract_power = 0.1;

	double exp_mul = 0;
	double exp_sign = 0;
	int has_exp = 0;

	if (*str == '-')
		state = STATE_MINUS;
	else if (*str == '0')
		state = STATE_ZERO;
	else
		state = STATE_DIGIT;
	if (state != STATE_DIGIT)
		str++;
	if (state == STATE_MINUS)
		sign = 1;
	
	while (*str && state != STATE_END) {
		if (state == STATE_MINUS) {
			if (*str && strchr("123456789", *str)) {
				val = val * 10 + (*str - '0');
				str++;
				state = STATE_DIGIT;
			}
			else if (*str == '0') {
				str++;
				state = STATE_ZERO;
			}
			else
				return 0;
		}
		else if (state == STATE_DIGIT) {
			if (*str && strchr("0123456789", *str)) {
				val = val * 10 + (*str - '0');
				str++;
			}
			else if (*str == '.') {
				str++;
				if (!*str || !strchr("0123456789", *str))
					return 1;
				state = STATE_FRACT;
			}
			else if (*str == 'e' || *str == 'E') {
				str++;
				state = STATE_EXP;
			}
			else
				state = STATE_END;
		}
		else if (state == STATE_FRACT) {
			if (*str && strchr("0123456789", *str)) {
				val = val + fract_power * (*str - '0');
				fract_power /= 10;
				str++;
			}
			else if (*str == 'e' || *str == 'E') {
				str++;
				state = STATE_EXP;
			}
			else
				state = STATE_END;
		}
		else if (state == STATE_EXP) {
			if (!has_exp) {
				if (*str == '-') {
					exp_sign = 1;
					str++;
				}
				else if (*str == '+')
					str++;
				if (!*str && !strchr("0123456789", *str))
					return 1;
				has_exp = 1;
			}
			if (*str && strchr("0123456789", *str)) {
				exp_mul = exp_mul * 10 + (*str - '0');
				str++;
			}
			else
				state = STATE_END;
		}
	}
	val += fract;
	if (has_exp) {
		exp_mul = get_10_power(exp_mul);
		if (exp_sign)
			exp_mul = 1 / exp_mul;
		val *= exp_mul;
	}
	if (sign)
		val = -val;

	*res = val;
	*advanced = str - start;
	return 0;
}

int _I_lexe_escape_string(char *str, int line) {
	size_t dest = 0;
	size_t src = 0;
	while (str[src]) {
		if (str[src] != '\\') {
			src++;
			dest++;
		}
		else {
			src++;
			int is_simple = 1;
			if (str[src] == '\\')
				str[dest] = '\\';
			else if (str[src] == '"')
				str[dest] = '"';
			else if (str[src] == '/')
				str[dest] = '/';
			else if (str[src] == 'b')
				str[dest] = '\b';
			else if (str[src] == 'f')
				str[dest] = '\f';
			else if (str[src] == 'n')
				str[dest] = '\n';
			else if (str[src] == 'r')
				str[dest] = '\r';
			else if (str[src] == 't')
				str[dest] = '\t';
			else if (str[src] == 'u') {
				is_simple = 0;
				src++;
				uint32_t codepoint = 0;
				for (int i = 0; i < 4; i++) {
					char c = str[src + i];
					if (!c || !strchr("0123456789abcdefABCDEF", c)) {
						fprintf(stderr, "error: expected  4-digit hex value after \\u line %d\n", line);
						return 1;
					}
					if ('0' <= c && c <= '9')
						codepoint = (codepoint << 4) | (c - '0');
					else if ('A' <= c && c <= 'F')
						codepoint = (codepoint << 4) | (c - 'A' + 0xa);
					else if ('a' <= c && c <= 'f')
						codepoint = (codepoint << 4) | (c - 'a' + 0xa);
				}
				src += 4;

				if (codepoint <= 0x7f) {
					str[dest] = codepoint;
					dest++;
				}
				else if (codepoint <= 0x7ff) {
					str[dest] = 0xc0 | (codepoint >> 6);
					str[dest + 1] = 0x80 | (codepoint & 0x3f);
					dest += 2;
				}
				else if (codepoint <= 0xffff) {
					str[dest] = 0xe0 | (codepoint >> 12);
					str[dest + 1] = 0x80 | ((codepoint >> 6) & 0x3f);
					str[dest + 2] = 0x80 | (codepoint & 0x3f);
					dest += 3;
				}
				else if (codepoint <= 0x10ffff) {
					str[dest] = 0xf0 | (codepoint >> 18);
					str[dest + 1] = 0x80 | ((codepoint >> 12) & 0x3f);
					str[dest + 2] = 0x80 | ((codepoint >> 6) & 0x3f);
					str[dest + 3] = 0x80 | (codepoint & 0x3f); 
					dest += 4;
				}
				else {
					fprintf(stderr, "error: unknown codepoint '%x' line %d\n", codepoint, line);
				}
				
			}
			else {
				fprintf(stderr, "error: unknown escape sequence '\\%c' line %d\n", str[src], line);
				return 1;
			}

			if (is_simple) {
				dest++;
				src++;
			}
		}
	}

	str[dest] = '\0';
	return 0;
}
