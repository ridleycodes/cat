### Call once to initialize the CRT
MACRO(ADD_CRT CRT_LOCATION)
    IF(NOT DEFINED CMAKE_CRT_INITIALIZED)
        SET(CMAKE_MSVC_RUNTIME_CHECKS "")
        IF(CMAKE_C_COMPILER_ID MATCHES "Clang")
            SET(CMAKE_CLANG_COMPILER ON)
        ENDIF(CMAKE_C_COMPILER_ID MATCHES "Clang")

        ### Handle cross compilation
        IF(NOT WIN32)
            MESSAGE(FATAL_ERROR "This program is for Windows targets only. Detected: ${CMAKE_SYSTEM_NAME}")
        ENDIF(NOT WIN32)
        IF(NOT CMAKE_CROSSCOMPILING)
            IF(MSVC)
                SET(CMAKE_LIB_COMPILER "lib.exe")
            ELSE(MSVC)
                SET(CMAKE_LIB_COMPILER "dlltool")
            ENDIF(MSVC)
        ### Add TARGET_ARCHITECTURE macro
            SET(CMAKE_MODULE_PATH ${CRT_LOCATION}/cmake ${CMAKE_MODULE_PATH})
            SET(CMAKE_TARGET_ARCH_FILE "${CRT_LOCATION}/cmake/arch.c")
            INCLUDE(TargetArch)
        ENDIF(NOT CMAKE_CROSSCOMPILING)

        OPTION(USE_OLD_CRT "Link to CRTDLL.DLL instead of MSVCRT.DLL (X86 only)" ON)

        ### Define global variables
        IF(NOT DEFINED EXECUTABLE_COMPILE_FLAGS)
            SET(EXECUTABLE_COMPILE_FLAGS "")
        ENDIF(NOT DEFINED EXECUTABLE_COMPILE_FLAGS)
        IF(NOT DEFINED EXECUTABLE_LINK_FLAGS)
            SET(EXECUTABLE_LINK_FLAGS "")
        ENDIF(NOT DEFINED EXECUTABLE_LINK_FLAGS)
        IF(NOT DEFINED EXECUTABLE_DEPENDENCIES)
            SET(EXECUTABLE_DEPENDENCIES "")
        ENDIF(NOT DEFINED EXECUTABLE_DEPENDENCIES)
        IF(NOT DEFINED DLL_COMPILE_FLAGS)
            SET(DLL_COMPILE_FLAGS "")
        ENDIF(NOT DEFINED DLL_COMPILE_FLAGS)
        IF(NOT DEFINED DLL_LINK_FLAGS)
            SET(DLL_LINK_FLAGS "")
        ENDIF(NOT DEFINED DLL_LINK_FLAGS)
        IF(NOT DEFINED DLL_DEPENDENCIES)
            SET(DLL_DEPENDENCIES "")
        ENDIF(NOT DEFINED DLL_DEPENDENCIES)
        IF(NOT DEFINED GLOBAL_COMPILE_FLAGS)
            SET(GLOBAL_COMPILE_FLAGS "")
        ENDIF(NOT DEFINED GLOBAL_COMPILE_FLAGS)
        IF(NOT DEFINED GLOBAL_LINK_FLAGS)
            SET(GLOBAL_LINK_FLAGS "")
        ENDIF(NOT DEFINED GLOBAL_LINK_FLAGS)
        IF(NOT DEFINED GLOBAL_ADDITIONAL_SOURCES)
            SET(GLOBAL_ADDITIONAL_SOURCES "")
        ENDIF(NOT DEFINED GLOBAL_ADDITIONAL_SOURCES)
        IF(NOT DEFINED GLOBAL_ADDITIONAL_LIBRARIES)
            SET(GLOBAL_ADDITIONAL_LIBRARIES "")
        ENDIF(NOT DEFINED GLOBAL_ADDITIONAL_LIBRARIES)

        ### Detect Architecture
        IF(NOT CMAKE_CROSSCOMPILING AND NOT DEFINED MACHINE_ARCH)
            SET(MACHINE_ARCH "X86")
            TARGET_ARCHITECTURE(DETECTED_ARCH)
            IF(DETECTED_ARCH MATCHES "ia64")
                SET(MACHINE_ARCH "IA64")
            ENDIF(DETECTED_ARCH MATCHES "ia64")
            IF(DETECTED_ARCH MATCHES "x86_64")
                SET(MACHINE_ARCH "X64")
            ENDIF(DETECTED_ARCH MATCHES "x86_64")
            IF(DETECTED_ARCH MATCHES "arm64")
                SET(MACHINE_ARCH "ARM64")
            ENDIF(DETECTED_ARCH MATCHES "arm64")
            IF(DETECTED_ARCH MATCHES "arm" AND NOT DETECTED_ARCH MATCHES "arm64")
                SET(MACHINE_ARCH "ARM")
            ENDIF(DETECTED_ARCH MATCHES "arm" AND NOT DETECTED_ARCH MATCHES "arm64")
        ENDIF(NOT CMAKE_CROSSCOMPILING AND NOT DEFINED MACHINE_ARCH)
        IF(NOT MSVC)
            STRING(TOLOWER "${MACHINE_ARCH}" GNU_MACHINE_ARCH)
            IF(MACHINE_ARCH STREQUAL "X86")
                SET(GNU_MACHINE_ARCH "i386")
            ENDIF(MACHINE_ARCH STREQUAL "X86")
            IF(MACHINE_ARCH STREQUAL "X64")
                SET(GNU_MACHINE_ARCH "i386:x86-64")
            ENDIF(MACHINE_ARCH STREQUAL "X64")
            IF(MACHINE_ARCH STREQUAL "ARM64")
                SET(GNU_MACHINE_ARCH "aarch64")
            ENDIF(MACHINE_ARCH STREQUAL "ARM64")
        ENDIF(NOT MSVC)
        IF(NOT MACHINE_ARCH STREQUAL "X86")
            SET(USE_OLD_CRT OFF)
        ENDIF(NOT MACHINE_ARCH STREQUAL "X86")

        ### Determine compile and link flags
        IF(MSVC)
            ### Definitions
            ADD_DEFINITIONS(-D_NO_CRT_STDIO_INLINE)     # Enable CRT standalone functions for UCRT headers
            ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)  # Disable warnings for the use of non *_s functions

            ### Compile Flags
            #SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} /O1")         # Optimize for size
            #SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} /W4 /WX")     # All warnings + treat warnings as errors

            IF(NOT MSVC_VERSION LESS 1300)
                SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} /GS-")    # Disable buffer security check
            ENDIF(NOT MSVC_VERSION LESS 1300)

            IF(NOT MSVC_VERSION LESS 1910)
                SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} /JMC-")   # Disable Just My Code debugging
            ENDIF(NOT MSVC_VERSION LESS 1910)

            #IF(NOT MSVC_VERSION LESS 1400)                                  # Precise floating point
            #    SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} /fp:precise")
            #ELSE(NOT MSVC_VERSION LESS 1400)
            #    SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} /Op")
            #ENDIF(NOT MSVC_VERSION LESS 1400)

            FOREACH(FLAG_VAR
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
                STRING(REGEX REPLACE "/RTC(su|[1su])" "" ${FLAG_VAR} "${${FLAG_VAR}}")  # Disable runtime error checks
                STRING(REGEX REPLACE "/W[123]" "" ${FLAG_VAR} "${${FLAG_VAR}}")         # Remove other warning levels
                #STRING(REGEX REPLACE "/O[2dgistxyp]" "" ${FLAG_VAR} "${${FLAG_VAR}}")   # Remove other optimization levels
                #STRING(REGEX REPLACE "/Ob[0123]" "" ${FLAG_VAR} "${${FLAG_VAR}}")
                STRING(REPLACE "/GZ" "" ${FLAG_VAR} "${${FLAG_VAR}}")                   # Disable stack frame runtime
                SET(${FLAG_VAR} "${${FLAG_VAR}}" CACHE STRING "" FORCE)                 # Affect entire project
            ENDFOREACH(FLAG_VAR)

            ### Link Flags
            SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} /NODEFAULTLIB")     # Remove default libraries
            #SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} /WX")               # Treat linker warnings as errors

            IF(NOT MSVC_VERSION LESS 1100)
                SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} /FIXED:NO")     # Disable fixed base address (for Win32s)
            ENDIF(NOT MSVC_VERSION LESS 1100)
            IF(NOT MSVC_VERSION LESS 1400)
                SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} /MANIFEST:NO")  # Disable manifest generation
            ENDIF(NOT MSVC_VERSION LESS 1400)

            IF((CMAKE_CLANG_COMPILER OR MSVC_VERSION LESS 1600) AND MACHINE_ARCH STREQUAL "X86")                                              # Windows NT 3.1 subsystem
                SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} /SUBSYSTEM:${EXECUTABLE_SUBSYSTEM},3.10 /OSVERSION:1.0 /VERSION:1.0")
            ELSE((CMAKE_CLANG_COMPILER OR MSVC_VERSION LESS 1600) AND MACHINE_ARCH STREQUAL "X86")   
                IF(MACHINE_ARCH STREQUAL "X86")                                                                     # Windows XP subsystem
                    SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} /SUBSYSTEM:${EXECUTABLE_SUBSYSTEM},5.01")
                ENDIF(MACHINE_ARCH STREQUAL "X86")
                IF(MACHINE_ARCH STREQUAL "X64" OR MACHINE_ARCH STREQUAL "IA64")                                     # Windows XP x64 subsystem
                    SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} /SUBSYSTEM:${EXECUTABLE_SUBSYSTEM},5.02")
                ENDIF(MACHINE_ARCH STREQUAL "X64" OR MACHINE_ARCH STREQUAL "IA64")
                IF(MACHINE_ARCH STREQUAL "ARM" OR MACHINE_ARCH STREQUAL "ARM64")                                    # Windows 8
                    SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} /SUBSYSTEM:${EXECUTABLE_SUBSYSTEM},6.02")
                ENDIF(MACHINE_ARCH STREQUAL "ARM" OR MACHINE_ARCH STREQUAL "ARM64")
            ENDIF((CMAKE_CLANG_COMPILER OR MSVC_VERSION LESS 1600) AND MACHINE_ARCH STREQUAL "X86")
        ELSE(MSVC)
            ### Definitions
            ADD_DEFINITIONS(-D__CRTDLL__)                                               # Disable UCRT functions if using UCRT compiler
            ### Compile Flags
            #SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} -Os")                     # Optimize for size
            #SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} -Wall -Werror")           # All warnings + treat warnings as errors

            SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} -mfpmath=387")            # Generate 386-compatible math
            SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} -fno-stack-protector")    # Disable stack protector
            SET(GLOBAL_COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} -fno-ident")              # Disable .ident section

            ### Link Flags
            SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} -nostdlib")                                                 # Remove default libraries
            SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} -pie")                                                      # Create position-independent code (for Win32s)
            #SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} -Wl,--warn-common -Wl,--warn-once -Wl,--fatal-warnings")    # Enable linker warnings and treat them as errors

            STRING(TOLOWER "${EXECUTABLE_SUBSYSTEM}" GNU_EXECUTABLE_SUBSYSTEM)
            IF(MACHINE_ARCH STREQUAL "X86")                                                                         # Windows NT 3.1 subsystem
                SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} -Wl,--subsystem,${GNU_EXECUTABLE_SUBSYSTEM}:3.10 -Wl,--major-os-version,1 -Wl,--minor-os-version,0")
            ENDIF(MACHINE_ARCH STREQUAL "X86")
            IF(MACHINE_ARCH STREQUAL "X64" OR MACHINE_ARCH STREQUAL "IA64")                                         # Windows XP x64 subsystem
                SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} -Wl,--subsystem,${GNU_EXECUTABLE_SUBSYSTEM}:5.02")
            ENDIF(MACHINE_ARCH STREQUAL "X64" OR MACHINE_ARCH STREQUAL "IA64")
            IF(MACHINE_ARCH STREQUAL "ARM" OR MACHINE_ARCH STREQUAL "ARM64")                                        # Windows 8
                SET(EXECUTABLE_LINK_FLAGS "${EXECUTABLE_LINK_FLAGS} -Wl,--subsystem,${GNU_EXECUTABLE_SUBSYSTEM}:6.02")
            ENDIF(MACHINE_ARCH STREQUAL "ARM" OR MACHINE_ARCH STREQUAL "ARM64")
        ENDIF(MSVC)

        ### Add CRT files
        SET(CMAKE_MODULE_PATH ${CRT_LOCATION}/src ${CMAKE_MODULE_PATH})
        INCLUDE(_CrtLibrary)
        ADD_CRT_LIBRARY(${CRT_LOCATION}/src)

        ### Add patcher_pe in case it is needed
        ADD_EXECUTABLE(patch_pe ${CRT_LOCATION}/cmake/patch_pe.c ${GLOBAL_ADDITIONAL_SOURCES})
        SET_TARGET_PROPERTIES(patch_pe PROPERTIES 
            COMPILE_FLAGS "${GLOBAL_COMPILE_FLAGS} ${EXECUTABLE_COMPILE_FLAGS}"
            LINK_FLAGS "${GLOBAL_LINK_FLAGS} ${EXECUTABLE_LINK_FLAGS}"
        )
        IF(GLOBAL_ADDITIONAL_LIBRARIES)
            TARGET_LINK_LIBRARIES(patch_pe ${GLOBAL_ADDITIONAL_LIBRARIES})
        ENDIF(GLOBAL_ADDITIONAL_LIBRARIES)
        IF(EXECUTABLE_DEPENDENCIES)
            ADD_DEPENDENCIES(patch_pe ${EXECUTABLE_DEPENDENCIES})
        ENDIF(EXECUTABLE_DEPENDENCIES)
        SET(CMAKE_CRT_INITIALIZED ON)
    ENDIF(NOT DEFINED CMAKE_CRT_INITIALIZED)
ENDMACRO(ADD_CRT)

### Add post logic macro
MACRO(ADD_CRT_EXECUTABLE EXECUTABLE_NAME)
    ### Custom build command: patch PE header
    # Clang is actually good and doesn't need this workaround
    IF(NOT CMAKE_CLANG_COMPILER AND MSVC AND NOT MSVC_VERSION LESS 1600 AND MACHINE_ARCH STREQUAL "X86")
        IF(COMMAND cmake_policy)
            SET(EXECUTABLE_LOCATION "$<TARGET_FILE:${EXECUTABLE_NAME}>")
            SET(PATCHER_LOCATION "$<TARGET_FILE:patch_pe>")
        ELSE(COMMAND cmake_policy)
            GET_TARGET_PROPERTY(EXECUTABLE_LOCATION ${EXECUTABLE_NAME} LOCATION)
            GET_TARGET_PROPERTY(PATCHER_LOCATION patch_pe LOCATION)
        ENDIF(COMMAND cmake_policy)

        ADD_CUSTOM_COMMAND(
            TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND "${PATCHER_LOCATION}" "${EXECUTABLE_LOCATION}"
        )
        ADD_DEPENDENCIES(${EXECUTABLE_NAME} patch_pe)
    ENDIF(NOT CMAKE_CLANG_COMPILER AND MSVC AND NOT MSVC_VERSION LESS 1600 AND MACHINE_ARCH STREQUAL "X86")
ENDMACRO(ADD_CRT_EXECUTABLE)