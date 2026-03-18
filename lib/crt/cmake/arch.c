#if defined(__aarch64__) || defined(_M_ARM64)
    #error cmake_ARCH ARM64
#elif defined(__arm__) || defined(_M_ARM)
        #error cmake_ARCH ARM
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #error cmake_ARCH X86
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #error cmake_ARCH X64
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
    #error cmake_ARCH IA64
#else
    #error cmake_ARCH X86
#endif