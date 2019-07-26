#pragma once
// Initializer/finalizer sample for MSVC and GCC/Clang.
    // 2010-2016 Joe Lowe. Released into the public domain.
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
    #define WRAPPY_INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#elif defined(_MSC_VER)
    #pragma section(".CRT$XCU",read)
    #define WRAPPY_INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
    #ifdef _WIN64
        #define WRAPPY_INITIALIZER(f) WRAPPY_INITIALIZER2_(f,"")
    #else
        #define WRAPPY_INITIALIZER(f) WRAPPY_INITIALIZER2_(f,"_")
    #endif
#else
    #define WRAPPY_INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif
