#ifndef _JOJ_RENDERER_H
#define _JOJ_RENDERER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "error_code.h"

typedef struct JRenderer_st
{
    void* data;
} JRenderer;

JAPI ErrorCode renderer_init(JRenderer* renderer);
JAPI void renderer_shutdown(JRenderer* renderer);

JAPI void renderer_print(JRenderer* renderer);

#endif // _JOJ_RENDERER_H