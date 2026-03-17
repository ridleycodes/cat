#include <stddef.h>

#include "newline.h"

LINE_ENDING strnnl(const char *str, size_t size) {
	size_t count_cr = 0, count_lf = 0, count_lf_cr = 0, count_cr_lf = 0, i = 0;

	if (str == NULL)
		return LINE_ENDING_UNKNOWN;

	while (i < size && str[i] != '\0') {
		if (i + 1 < size && str[i] == '\r' && str[i + 1] == '\n') {
			i += 2;
			count_cr_lf++;
		} else if (i + 1 < size && str[i] == '\n' && str[i + 1] == '\r') {
			i += 2;
			count_lf_cr++;
		} else if (str[i] == '\n') {
			i++;
			count_lf++;
		} else if (str[i] == '\r') {
			i++;
			count_cr++;
		} else
			i++;
	}

	i = count_cr | count_lf | count_cr_lf | count_lf_cr;
	if (i == 0) return LINE_ENDING_UNKNOWN;
	else if (i == count_cr) return LINE_ENDING_CR;
	else if (i == count_lf) return LINE_ENDING_LF;
	else if (i == count_cr_lf) return LINE_ENDING_CR_LF;
	else if (i == count_lf_cr) return LINE_ENDING_LF_CR;
	else return LINE_ENDING_MIXED;
}

LINE_ENDING strnl(const char *str) {
	size_t count_cr = 0, count_lf = 0, count_lf_cr = 0, count_cr_lf = 0, i = 0;

	if (str == NULL)
		return LINE_ENDING_UNKNOWN;

	while (str[i] != '\0') {
		if (str[i + 1] != '\0' && str[i] == '\r' && str[i + 1] == '\n') {
			i += 2;
			count_cr_lf++;
		} else if (str[i + 1] != '\0' && str[i] == '\n' && str[i + 1] == '\r') {
			i += 2;
			count_lf_cr++;
		} else if (str[i] == '\n') {
			i++;
			count_lf++;
		} else if (str[i] == '\r') {
			i++;
			count_cr++;
		} else
			i++;
	}

	i = count_cr | count_lf | count_cr_lf | count_lf_cr;
	if (i == 0) return LINE_ENDING_UNKNOWN;
	else if (i == count_cr) return LINE_ENDING_CR;
	else if (i == count_lf) return LINE_ENDING_LF;
	else if (i == count_cr_lf) return LINE_ENDING_CR_LF;
	else if (i == count_lf_cr) return LINE_ENDING_LF_CR;
	else return LINE_ENDING_MIXED;
}

int strnlfmt(const char *input, size_t cbinput, char *output, size_t *cboutput, LINE_ENDING fmt) {
	if (cboutput == NULL || fmt == LINE_ENDING_MIXED || fmt == LINE_ENDING_UNKNOWN)
		return STRING_LINE_FORMAT_INVALID_PARAMETERS;
	if (input == NULL || cbinput == 0) {
		*cboutput = 0;
		return STRING_LINE_FORMAT_SUCCESS;
	} else {
		size_t i = 0, j = 0;
		int err = STRING_LINE_FORMAT_SUCCESS;

		while (i < cbinput && input[i] != '\0') {
			int is_newline = 0;

			if (i + 1 < cbinput && input[i] == '\r' && input[i + 1] == '\n') {
				is_newline = 1; i += 2;
			} else if (i + 1 < cbinput && input[i] == '\n' && input[i + 1] == '\r') {
				is_newline = 1; i += 2;
			} else if (input[i] == '\n' || input[i] == '\r') {
				is_newline = 1; i += 1;
			}

			if (is_newline) {
				const char *nl_chars;
				size_t nl_len, k;

				switch (fmt) {
					case LINE_ENDING_CR: nl_chars = "\r"; nl_len = 1; break;
					case LINE_ENDING_LF: nl_chars = "\n"; nl_len = 1; break;
					case LINE_ENDING_CR_LF: nl_chars = "\r\n"; nl_len = 2; break;
					case LINE_ENDING_LF_CR: nl_chars = "\n\r"; nl_len = 2; break;
					default: return STRING_LINE_FORMAT_INVALID_PARAMETERS;
				}

				for (k = 0; k < nl_len; k++) {
					if (output != NULL && err == STRING_LINE_FORMAT_SUCCESS) {
						if (j < *cboutput) output[j] = nl_chars[k];
						else err = STRING_LINE_FORMAT_OUT_OF_MEMORY;
					}
					j++;
				}
			} else {
				if (output != NULL && err == STRING_LINE_FORMAT_SUCCESS) {
					if (j < *cboutput) output[j] = input[i];
					else err = STRING_LINE_FORMAT_OUT_OF_MEMORY;
				}
				j++;
				i++;
			}
		}

		if (output != NULL && err == STRING_LINE_FORMAT_SUCCESS) {
			if (j < *cboutput) output[j] = '\0';
			else err = STRING_LINE_FORMAT_OUT_OF_MEMORY;
		}
		j++;

		*cboutput = j;
		return err;
	}
}
