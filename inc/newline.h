#ifndef NEWLINE_H
#define NEWLINE_H

typedef enum LINE_ENDING_ENUM {
	LINE_ENDING_CR,
	LINE_ENDING_LF,
	LINE_ENDING_CR_LF,
	LINE_ENDING_LF_CR,
	LINE_ENDING_MIXED,
	LINE_ENDING_UNKNOWN
} LINE_ENDING;

/* determine line endings for a string, returns unknown for null and mixed for any non uniform string */
LINE_ENDING strnl(const char *str);

/* determine line endings for a sized string, returns unknown for null and mixed for any non uniform string */
LINE_ENDING strnnl(const char *str, size_t size);

typedef enum STRING_LINE_FORMAT_ENUM {
	STRING_LINE_FORMAT_SUCCESS,
	STRING_LINE_FORMAT_INVALID_PARAMETERS,
	STRING_LINE_FORMAT_OUT_OF_MEMORY
} STRING_LINE_FORMAT;

/* normalizes line endings to a given format
inputs:
	input: the string to be normalized.
	cbinput: size in bytes of the string.
	output: the output string. this could be the same as the input string unless more memory is needed. if null, the amount of memory needed by the function is returned in cboutput, and this parameter is ignored.
	cboutput: size of the output string in bytes.
	fmt: format. must be one of the valid LINE_ENDING values.
output: one of the success or error codes from STRING_LINE_FORMAT.
*/
int strnlfmt(const char *input, size_t cbinput, char *output, size_t *cboutput, LINE_ENDING fmt);

#endif /* NEWLINE_H */