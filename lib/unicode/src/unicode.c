#include <stddef.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "unicode.h"

#ifndef _O_U16TEXT
#define _O_U16TEXT     0x20000
#endif /* _O_U16TEXT */

int uprintf(wchar_t *fmt, ...) {
	va_list args;
	int k, mode;

	if (fflush(stdout) < 0)
		return -1;
	mode = _setmode(_fileno(stdout), _O_U16TEXT);

	va_start(args, fmt);
	k = vwprintf(fmt, args);
	va_end(args);

	if (fflush(stdout) < 0)
		k = -1;
	/* _setmode might not have been necessary */
	if (mode >= 0)
		_setmode(_fileno(stdout), mode);

	return k;
}

int uvprintf(wchar_t *fmt, va_list args) {
	int k, mode;

	if (fflush(stdout) < 0)
		return -1;
	mode = _setmode(_fileno(stdout), _O_U16TEXT);

	k = vwprintf(fmt, args);

	if (fflush(stdout) < 0)
		k = -1;
	/* _setmode might not have been necessary */
	if (mode >= 0)
		_setmode(_fileno(stdout), mode);

	return k;
}

size_t utf8strlen(const char *stri) {
	const unsigned char *str = (const unsigned char*) stri;
	size_t i;
	size_t count;

	if (str == NULL)
		return 0;

	/* detect BOM */
	i = (str[0] != '\0' && str[1] != '\0' && str[2] != '\0' && str[0] == 0xEF && str[1] == 0xBB && str[2] == 0xBF) ? 3 : 0;
	count = (i == 3) ? 1 : 0;

	while (str[i] != '\0') {
		/* check for intermediate unicode char */
		if ((str[i] & 0xC0) != 0x80)
			++count;
		++i;
	}

	return count;
}

size_t utf8strnlen(const char *stri, size_t size) {
	const unsigned char *str = (const unsigned char*) stri;
	size_t i;
	size_t count;

	if (str == NULL)
		return 0;

	/* detect BOM */
	i = (size >= 3 && str[0] != '\0' && str[1] != '\0' && str[2] != '\0' && str[0] == 0xEF && str[1] == 0xBB && str[2] == 0xBF) ? 3 : 0;
	count = (i == 3) ? 1 : 0;

	while (i < size && str[i] != '\0') {
		/* check for intermediate unicode char */
		if ((str[i] & 0xC0) != 0x80)
			++count;
		++i;
	}

	return count;
}

int utf8toucs2(const char *inputi, size_t cbinput, unsigned char *output, size_t *cboutput, int bigendian) {
	const unsigned char *input = (const unsigned char*) inputi;
	if (cboutput == NULL)
		return UTF_CONVERSION_INVALID_PARAMETERS;
	if (input == NULL) {
		*cboutput = 0;
		return UTF_CONVERSION_SUCCESS;
	} else {
		/* convert codepoints of UTF-8 of at most 2 bytes to their corresponding UCS-2 chars */
		/* for 4 byte chars, represent them by U+FFFD or 0xFD followed by 0xFF */

		/* high is read first in an utf-8 stream with special attention to continuation bits */
		unsigned char low_byte = 0, high_byte = 0;
		size_t count_ucs2 = 0, i = 0, j = 0;
		int err = UTF_CONVERSION_SUCCESS;

		/* detect BOM */
		if (cbinput >= 3 && input[0] != '\0' && input[1] != '\0' && input[2] != '\0' && input[0] == 0xEF && input[1] == 0xBB && input[2] == 0xBF) {
			i = 3;
			count_ucs2 = 1;
			if (output != NULL) {
				if (j + 1 < *cboutput) {
					output[j++] = (unsigned char) (bigendian ? 0xFE : 0xFF);
					output[j++] = (unsigned char) (bigendian ? 0xFF : 0xFE);
				} else {
					return UTF_CONVERSION_OUT_OF_MEMORY;
				}
			}
		}

		while (i < cbinput && input[i] != '\0') {
			++count_ucs2;
			/* evaluate sequence length via leading byte */
			switch(input[i] & 0xF0) {
				case 0x80:
				case 0x90:
				case 0xA0:
				case 0xB0:
					err = UTF_CONVERSION_MALFORMED_INPUT;
					break;
				case 0xF0:
					if (i + 4 > cbinput
					|| 	(input[i + 1] & 0xC0) != 0x80
					||	(input[i + 2] & 0xC0) != 0x80
					||	(input[i + 3] & 0xC0) != 0x80
					/* security check for valid range */
					||	input[i] > 0xF4
					||	(input[i] == 0xF0 && input[i + 1] < 0x90)
					||	(input[i] == 0xF4 && input[i + 1] > 0x8F)) {
						err = UTF_CONVERSION_MALFORMED_INPUT;
						break;
					}
					if (err == UTF_CONVERSION_SUCCESS)
						err = UTF_UCS2_MISSING_CHARACTERS;
					i += 4;
					low_byte = 0xFD; high_byte = 0xFF;
					break;
				case 0xE0:
					if (i + 3 > cbinput
					|| 	(input[i + 1] & 0xC0) != 0x80
					|| 	(input[i + 2] & 0xC0) != 0x80
					/* security check for valid range */
					|| 	(input[i] == 0xE0 && input[i + 1] < 0xA0)
					|| 	(input[i] == 0xED && input[i + 1] > 0x9F)) {
						err = UTF_CONVERSION_MALFORMED_INPUT;
						break;
					}
					low_byte = (unsigned char) (((input[i + 1] & 0x03) << 6) | (input[i + 2] & 0x3F));
					high_byte = (unsigned char) (((input[i] & 0x0F) << 4) | ((input[i + 1] & 0x3C) >> 2));
					i += 3;
					break;
				case 0xC0:
				case 0xD0:
					if (i + 2 > cbinput
					|| 	(input[i + 1] & 0xC0) != 0x80
					/* security check for valid range */
					|| 	input[i] < 0xC2) {
						err = UTF_CONVERSION_MALFORMED_INPUT;
						break;
					}
					low_byte = (unsigned char) (((input[i] & 0x03) << 6) | (input[i + 1] & 0x3F));
					high_byte = (unsigned char) ((input[i] & 0x1C) >> 2);
					i += 2;
					break;
				default:
					low_byte = (unsigned char) input[i]; high_byte = 0;
					++i;
					break;
			}
			if (err == UTF_CONVERSION_MALFORMED_INPUT)
				break;
			if (output != NULL && err != UTF_CONVERSION_OUT_OF_MEMORY) {
				if (j + 1 < *cboutput) {
					output[j++] = (unsigned char) (bigendian ? high_byte : low_byte);
					output[j++] = (unsigned char) (bigendian ? low_byte : high_byte);
				} else {
					err = UTF_CONVERSION_OUT_OF_MEMORY;
				}
			}
		}
		if (err == UTF_CONVERSION_MALFORMED_INPUT)
			return err;

		/* encode the null */
		if (i < cbinput && input[i] == '\0') {
			++count_ucs2;
			/* encode the null */
			if (output != NULL) {
				if (j + 1 < *cboutput) {
					output[j++] = '\0';
					output[j++] = '\0';
				} else {
					err = UTF_CONVERSION_OUT_OF_MEMORY;
				}
			}
		}

		/* every UCS-2 character requires 2 bytes */
		*cboutput = count_ucs2 * 2;
		return err;
	}
}
