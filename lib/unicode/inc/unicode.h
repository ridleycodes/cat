/* 
	Unicode Library, writen in standard C89.
*/
#ifndef UNICODE_H
#define UNICODE_H

#if defined(_WIN32) && defined(UNICODE_LIB_SHARED)
    #ifdef UNICODE_EXPORT
        #define UNICODE_API __declspec(dllexport)
    #else /* UNICODE_EXPORT */
        #define UNICODE_API __declspec(dllimport)
    #endif /* UNICODE_EXPORT */
#else /* defined(_WIN32) && defined(UNICODE_LIB_SHARED) */
    #define UNICODE_API
#endif /* defined(_WIN32) && defined(UNICODE_LIB_SHARED) */

#include <stdarg.h>

/* identifier for uprintf and uvprintf */
#ifndef LEGACY_CRT
#define USTR_FMT L"%ls"
#else /* LEGACY_CRT */
#define USTR_FMT L"%s"
#endif /* LEGACY_CRT */

/* 
	correctly prints unicode by flushing buffers and changing modes before printing and restoring
		them after printing.
*/
UNICODE_API int uprintf(wchar_t *fmt, ...);

/* 
	correctly prints unicode by flushing buffers and changing modes before printing and restoring
		them after printing.
*/
UNICODE_API int uvprintf(wchar_t *fmt, va_list args);

/* 
	returns the size of a UTF-8 string in characters. BOM is detected and counted.
	inputs:
		str: the string. must be null terminated.
*/
UNICODE_API size_t utf8strlen(const char *str);

/* 
	returns the size of a UTF-8 string in characters. BOM is detected and counted.
	inputs:
		str: the string.
		size: cuts the reading of str at the desired number of bytes, or stops at a null terminator
*/
UNICODE_API size_t utf8strnlen(const char *str, size_t size);

/* codes for the different utf conversion functions. */
typedef enum UTF_CONVERSION_CODE_ENUM {
    UTF_CONVERSION_SUCCESS,
    UTF_UCS2_MISSING_CHARACTERS,
    UTF_CONVERSION_INVALID_PARAMETERS,
    UTF_CONVERSION_MALFORMED_INPUT,
    UTF_CONVERSION_OUT_OF_MEMORY
} UTF_CONVERSION_CODE;

/* 
	converts UTF-8 to UCS-2
	inputs:
		input: input array of UTF-8 bytes. BOM is detected and rewritten at output if present. if input is NULL, then utf8toucs2 unconditionally returns UTF_CONVERSION_SUCCESS.
		cbinput: size of the array input in bytes.
		output: output array of UCS-2 bytes. this should be a pointer with the correct size to prevent buffer overflow.
		cboutput: size of the array output in bytes. if output pointer is null, the needed size for the output is returned in cboutput.
		bigendian: if true, output is in big endian. else, it is in little endian.
	output: error or success code from UTF_CONVERSION_CODE.
*/
UNICODE_API int utf8toucs2(const char *input, size_t cbinput, unsigned char *output, size_t *cboutput, int bigendian);

#endif /* UNICODE_H */
