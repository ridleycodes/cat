#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <locale.h>

#ifdef UNICODE_LIB
#include "unicode.h"
#endif /* UNICODE_LIB */
#include "newline.h"

int main(int argc, char *argv[]) {
	FILE *fp;
	size_t bytes;
	unsigned char *buf;
	int incorrect_codepage = 0, k;

	setlocale(LC_ALL,"");

	if (argc < 2) {
		printf("Usage: %s <path of file to read>\n", argv[0]);
		return 0;
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "File does not exist or cannot be opened.\n");
		return 0x3;
	}

	/* obtain file size and allocate memory */
	fseek(fp, 0L, SEEK_END);
	bytes = (size_t) ftell(fp);
	buf = (unsigned char*) malloc(bytes + 2);
	if (buf == NULL) {
		fclose(fp);
		fprintf(stderr, "Insufficient memory available.\n");
		return 0x8;
	}

	fseek(fp, 0L, SEEK_SET);
	/* read file into memory */
	if (fread((void*) buf, sizeof(unsigned char), bytes, fp) < bytes) {
		fclose(fp);
		free(buf);
		fprintf(stderr, "An unknown error occurred while reading the file.\n");
		return INT_MIN;
	}

	buf[bytes] = '\0';
	buf[bytes+1] = '\0';

#ifdef UNICODE_LIB
	/* if UTF-8, convert it to wide bytes */
	if (bytes >= 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
		/* get needed size */
		size_t bytes_out;
		wchar_t *out;
		int err = utf8toucs2((const char*) buf, bytes + 2, NULL, &bytes_out, 0);
		if (err == UTF_CONVERSION_INVALID_PARAMETERS) {
			fclose(fp);
			free(buf);
			fprintf(stderr, "An unknown error occurred while reading the file.\n");
			return INT_MIN;
		} else if (err == UTF_CONVERSION_MALFORMED_INPUT) {
			fclose(fp);
			free(buf);
			fprintf(stderr, "The UTF-8 input file is malformed and cannot be read.\n");
			return 0xD;
		}
		out = (wchar_t*) malloc(bytes_out);
		if (out == NULL) {
			fclose(fp);
			free(buf);
			fprintf(stderr, "Insufficient memory available.");
			return 0x8;
		}
		err = utf8toucs2((const char*) buf, bytes + 2, (unsigned char*) out, &bytes_out, 0);
		if (err == UTF_CONVERSION_INVALID_PARAMETERS) {
			fclose(fp);
			free(buf);
			free(out);
			fprintf(stderr, "An unknown error occurred while reading the file.\n");
			return INT_MIN;
		}
		k = uprintf(USTR_FMT, out+1);
		if (k < 0) {
			fclose(fp);
			free(buf);
			free(out);
			fprintf(stderr, "An unknown error occurred while writing the output.\n");
			return INT_MIN;
		}
		if (k < (int) (utf8strnlen((const char*) (buf + 3), bytes - 1)))
			incorrect_codepage = 1;
		free(out);
	/* it is UTF-16LE */
	} else if (bytes >= 2 && buf[0] == 0xFF && buf[1] == 0xFE) {
		k = uprintf(USTR_FMT, ((wchar_t*) buf) + 1);
		if (k < 0) {
			fclose(fp);
			free(buf);
			fprintf(stderr, "An unknown error occurred while writing the output.\n");
			return INT_MIN;
		}
		if (k < (((int) bytes / 2) - 1))
			incorrect_codepage = 1;
	/* it is ANSI */
	} else {
#else /* UNICODE_LIB */
	{
#endif /* UNICODE_LIB */
		k = strnlfmt((char*) buf, bytes + 2, (char*) buf, &bytes, LINE_ENDING_LF);
		if (k < 0) {
			fclose(fp);
			free(buf);
			fprintf(stderr, "An unknown error occurred while writing the output.\n");
			return INT_MIN;
		}
		k = printf("%s", (char*) buf);
		if (k < 0) {
			fclose(fp);
			free(buf);
			fprintf(stderr, "An unknown error occurred while writing the output.\n");
			return INT_MIN;
		}
		/* printf won't print the null separator */
		if (k + 1 < (int) bytes)
			incorrect_codepage = 1;
	}
	if (incorrect_codepage)
		fprintf(stderr, "\nWARNING: Codepage does not match input file, or characters fall outside representable range. Some characters were not displayed.\n");
	else
		printf("\n");
	fclose(fp);
	free(buf);
	return 0;
}
