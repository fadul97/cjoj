#ifndef _JOJ_RENDERER_H
#define _JOJ_RENDERER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "error_code.h"

JAPI ErrorCode renderer_init();
JAPI void renderer_shutdown();

JAPI void renderer_print();

#endif // _JOJ_RENDERER_H