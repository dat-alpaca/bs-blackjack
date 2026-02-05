#pragma once
#include <stdio.h>

#if defined(WIN32) || defined(_WIN32)
	#define TUR_PLATFORM_WIN32
	#define TUR_BREAKPOINT() __debugbreak()
#elif defined(__linux__)
	#define TUR_PLATFORM_LINUX
	#define TUR_BREAKPOINT() __builtin_trap()
#elif defined(__APPLE__)
	#define TUR_PLATFORM_APPLE
	#define TUR_BREAKPOINT() __builtin_trap()
#elif defined(__ANDROID__)
	#define TUR_PLATFORM_ANDROID
	#define TUR_BREAKPOINT() __builtin_trap()
#else
	#error Unsupported platform
#endif

#define BS_ASSERT_MESSAGE(condition, message)                               \
{                                                                           \
    if(condition) { }                                                       \
    else                                                                    \
    {                                                                       \
        fprintf(stderr, message " " #condition);                            \
        fprintf(stderr, "File: %s (line %d)", __FILE__, __LINE__);          \
        TUR_BREAKPOINT();                                                   \
    }                                                                       \
}

#define BS_ASSERT_NULL(condition)                                           \
{                                                                           \
    if(condition) { }                                                       \
    else                                                                    \
    {                                                                       \
        fprintf(stderr, "Null reference: %s", #condition);                  \
        fprintf(stderr, "File: %s (line %d)", __FILE__, __LINE__);          \
        TUR_BREAKPOINT();                                                   \
    }                                                                       \
}