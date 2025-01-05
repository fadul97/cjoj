#ifndef _JOJ_RENDERER_H
#define _JOJ_RENDERER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "error_code.h"

struct JRenderer;

JAPI ErrorCode renderer_init(struct JRenderer** renderer);
JAPI void renderer_shutdown(struct JRenderer** renderer);

JAPI void renderer_print(struct JRenderer** renderer);

#endif // _JOJ_RENDERER_H