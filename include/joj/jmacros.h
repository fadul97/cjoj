#ifndef _JOJ_MACROS_H_
#define _JOJ_MACROS_H_

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JOJ_DEBUG_MODE

#ifndef JOJ_ASSERT
#include <stdio.h>
#include <stdlib.h>

#define JOJ_ASSERT(condition, ...)                                                                   \
    do {                                                                                             \
        if (!(condition)) {                                                                          \
            fprintf(stderr,                                                                          \
                    "Assertion failed: (%s), function %s, file %s, line %d.\n",                      \
                    #condition, __func__, __FILE__, __LINE__);                                       \
            fprintf(stderr, "Message: " __VA_ARGS__);                                                \
            fprintf(stderr, "\n");                                                                   \
            abort();                                                                                 \
        }                                                                                            \
    } while (0)
#endif // JOJ_ASSERT

#else // JOJ_DEBUG_MODE not defined (Release mode)

#define JOJ_ASSERT(condition, ...)

#endif // JOJ_DEBUG_MODE

#endif // _JOJ_MACROS_H_