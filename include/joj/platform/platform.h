#ifndef JOJ_PLATFORM_H
#define JOJ_PLATFORM_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "error_code.h"

typedef enum WindowMode_enum { BORDERLESS, FULLSCREEN, WINDOWED } WindowMode;

struct JWindow;

typedef struct JPlatformManager_st
{
    struct JWindow* window;
} JPlatformManager;

JAPI ErrorCode platform_init(JPlatformManager* platform_manager, u16 width, u16 height, const char* title, WindowMode mode);
JAPI void platform_shutdown(JPlatformManager* platform_manager);
JAPI b8 platform_process_events(JPlatformManager* platform_manager);

JAPI void print_from_joj();

#endif // JOJ_PLATFORM_H