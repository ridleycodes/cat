#ifndef _FILE_DEFINED
#define _FILE_DEFINED
typedef struct _iobuf {
	char *_ptr;
	int   _cnt;
	char *_base;
	int   _flag;
	int   _file;
	int   _charbuf;
	int   _bufsiz;
	char *_tmpfname;
} FILE;
#endif /* _FILE_DEFINED */
extern __declspec(dllimport) FILE _iob[];
FILE* __cdecl __iob_func(void) { return _iob; }
FILE* (__cdecl * _imp___iob_func)(void) = __iob_func;
FILE* (__cdecl * _imp____iob_func)(void) = __iob_func;
FILE* (__cdecl * __imp___iob_func)(void) = __iob_func;
FILE* (__cdecl * __imp____iob_func)(void) = __iob_func;
FILE* __cdecl __acrt_iob_func(unsigned i) { return _iob + i; }
FILE* (__cdecl * _imp___acrt_iob_func)(unsigned) = __acrt_iob_func;
FILE* (__cdecl * _imp____acrt_iob_func)(unsigned) = __acrt_iob_func;
FILE* (__cdecl * __imp___acrt_iob_func)(unsigned) = __acrt_iob_func;
FILE* (__cdecl * __imp____acrt_iob_func)(unsigned) = __acrt_iob_func;

#ifdef CRT_IS_DLL
extern int __stdcall DllMain(void* hinstDLL, unsigned long fdwReason, void* lpReserved);
int __stdcall DllMainCRTStartup(void* hinstDLL, unsigned long fdwReason, void* lpReserved) {    
    return DllMain(hinstDLL, fdwReason, lpReserved);
}
#else /* CRT_IS_DLL */
int main(int argc, char* argv[]);

#ifdef USE_OLD_CRT
extern __declspec(dllimport) void __cdecl __GetMainArgs(int *argc, char ***argv, char ***env, int dowildcard);
#define MAINARGS_CALL(argc, argv, envp, wild, startup) __GetMainArgs(argc, argv, envp, wild)
#else /* USE_OLD_CRT */
extern __declspec(dllimport) int __cdecl __getmainargs(int *argc, char ***argv, char ***env, int dowildcard, int *startupinfo);
#define MAINARGS_CALL(argc, argv, envp, wild, startup) __getmainargs(argc, argv, envp, wild, startup)
#endif /* USE_OLD_CRT */
extern __declspec(dllimport) void __declspec(noreturn) _exit(int status);

#ifdef __GNUC__
void __main(void) {}
#endif /* __GNUC__ */

void __cdecl mainCRTStartup(void) {
    int argc;
    char** argv;
    char** envp;
    int err;

    MAINARGS_CALL(&argc, &argv, &envp, 0, &err);

    err = main(argc, argv);

    _exit(err);
}
#endif /* CRT_IS_DLL */