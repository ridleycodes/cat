MACRO(TARGET_ARCHITECTURE output_var)
    SET(ARCH "unknown")

    IF(APPLE AND CMAKE_OSX_ARCHITECTURES)
        FOREACH(osx_arch ${CMAKE_OSX_ARCHITECTURES})
            IF("${osx_arch}" STREQUAL "ppc" AND ppc_support)
                SET(osx_arch_ppc TRUE)
            ENDIF("${osx_arch}" STREQUAL "ppc" AND ppc_support)
            IF("${osx_arch}" STREQUAL "i386")
                SET(osx_arch_i386 TRUE)
            ENDIF("${osx_arch}" STREQUAL "i386")
            IF("${osx_arch}" STREQUAL "x86_64")
                SET(osx_arch_x86_64 TRUE)
            ENDIF("${osx_arch}" STREQUAL "x86_64")
            IF("${osx_arch}" STREQUAL "ppc64" AND ppc_support)
                SET(osx_arch_ppc64 TRUE)
            ENDIF("${osx_arch}" STREQUAL "ppc64" AND ppc_support)
        ENDFOREACH(osx_arch)

        SET(ARCH "")
        IF(osx_arch_ppc)
            SET(ARCH "${ARCH} ppc")
        ENDIF(osx_arch_ppc)
        IF(osx_arch_i386)
            SET(ARCH "${ARCH} i386")
        ENDIF(osx_arch_i386)
        IF(osx_arch_x86_64)
            SET(ARCH "${ARCH} x86_64")
        ENDIF(osx_arch_x86_64)
        IF(osx_arch_ppc64)
            SET(ARCH "${ARCH} ppc64")
        ENDIF(osx_arch_ppc64)
    ELSE(APPLE AND CMAKE_OSX_ARCHITECTURES)
        CONFIGURE_FILE(${CMAKE_TARGET_ARCH_FILE} "${CMAKE_BINARY_DIR}/arch.c" COPYONLY)
        TRY_COMPILE(
            compile_result_unused
            "${CMAKE_BINARY_DIR}"
            "${CMAKE_BINARY_DIR}/arch.c"
            OUTPUT_VARIABLE COMPILER_OUTPUT
        )

        STRING(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH_MATCH "${COMPILER_OUTPUT}")

        IF(ARCH_MATCH)
            STRING(REPLACE "cmake_ARCH " "" ARCH "${ARCH_MATCH}")
        ELSE(ARCH_MATCH)
            SET(ARCH "unknown")
        ENDIF(ARCH_MATCH)
    ENDIF(APPLE AND CMAKE_OSX_ARCHITECTURES)

    SET(${output_var} "${ARCH}")
ENDMACRO(TARGET_ARCHITECTURE)