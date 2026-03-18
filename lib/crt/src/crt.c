#ifdef __LEGACY_CRT_CONSOLE_SUBSYSTEM
#if !defined (_MSC_VER) || _MSC_VER > 1200
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
#endif /* !defined (_MSC_VER) || _MSC_VER > 1200 */
#endif /* __LEGACY_CRT_CONSOLE_SUBSYSTEM */
#ifdef _MSC_VER
int _fltused = 1;
#endif /* _MSC_VER */

#ifdef __LEGACY_CRT_CRT_IS_DLL
extern int __stdcall DllMain(void* hinstDLL, unsigned long fdwReason, void* lpReserved);
int __stdcall DllMainCRTStartup(void* hinstDLL, unsigned long fdwReason, void* lpReserved) {    
    return DllMain(hinstDLL, fdwReason, lpReserved);
}
#else /* __LEGACY_CRT_CRT_IS_DLL */
#ifdef __LEGACY_CRT_USE_OLD_CRT
extern __declspec(dllimport) void __cdecl __GetMainArgs(int *argc, char ***argv, char ***env, int dowildcard);
#define MAINARGS_CALL(argc, argv, envp, wild, startup) __GetMainArgs(argc, argv, envp, wild)
#else /* __LEGACY_CRT_USE_OLD_CRT */
extern __declspec(dllimport) int __cdecl __getmainargs(int *argc, char ***argv, char ***env, int dowildcard, int *startupinfo);
#define MAINARGS_CALL(argc, argv, envp, wild, startup) __getmainargs(argc, argv, envp, wild, startup)
#endif /* __LEGACY_CRT_USE_OLD_CRT */
extern __declspec(dllimport) void __declspec(noreturn) _exit(int status);

#ifdef __LEGACY_CRT_CONSOLE_SUBSYSTEM
int main(int argc, char* argv[]);
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
#elif defined(__LEGACY_CRT_WINDOWS_SUBSYSTEM)
#if	!defined(_MSC_VER) || _MSC_VER > 1000
#endif /* !defined(_MSC_VER) || _MSC_VER > 1000 */
extern int __stdcall WinMain(void* hInstance, void* hPrevInstance, const char *lpCmdLine, int nCmdShow);
extern __declspec(dllimport) void* __stdcall GetModuleHandleA(const char* lpModuleName);
extern __declspec(dllimport) char* __stdcall GetCommandLineA(void);
extern __declspec(dllimport) void __stdcall GetStartupInfoA(void* lpStartupInfo);
extern __declspec(dllimport) void __stdcall ExitProcess(unsigned int status);

void __cdecl mainCRTStartup(void) {
	void* hInstance;
    char* lpszCommandLine;
    int nShowCmd;
    int result;

    struct {
        unsigned long cb;
        char* lpReserved;
        char* lpDesktop;
        char* lpTitle;
        unsigned long dwX;
        unsigned long dwY;
        unsigned long dwXSize;
        unsigned long dwYSize;
        unsigned long dwXCountChars;
        unsigned long dwYCountChars;
        unsigned long dwFillAttribute;
        unsigned long dwFlags;
        unsigned short wShowWindow;
        unsigned short cbReserved2;
        unsigned char* lpReserved2;
        void* hStdInput;
        void* hStdOutput;
        void* hStdError;
    } startupInfo;

    hInstance = GetModuleHandleA(0);
    lpszCommandLine = GetCommandLineA();
    if (*lpszCommandLine == '"') {
        lpszCommandLine++;
        while (*lpszCommandLine && *lpszCommandLine != '"') lpszCommandLine++;
        if (*lpszCommandLine == '"') lpszCommandLine++;
    } else
        while (*lpszCommandLine > ' ') lpszCommandLine++;
    while (*lpszCommandLine && *lpszCommandLine <= ' ') lpszCommandLine++;

    startupInfo.cb = sizeof(startupInfo);
    GetStartupInfoA(&startupInfo);
    nShowCmd = (startupInfo.dwFlags & 1) ? startupInfo.wShowWindow : 10;

    result = WinMain(hInstance, 0, lpszCommandLine, nShowCmd);

    ExitProcess(result);
}
#endif
#endif /* __LEGACY_CRT_CRT_IS_DLL */