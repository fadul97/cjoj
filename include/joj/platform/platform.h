#ifndef JOJ_PLATFORM_H
#define JOJ_PLATFORM_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "error_code.h"

#include <Windows.h>
#include <windowsx.h>

typedef enum WindowMode_enum { BORDERLESS, FULLSCREEN, WINDOWED } WindowMode;

struct JWindow;

typedef struct JPlatformManager_st
{
    struct JWindow* window;
} JPlatformManager;

LRESULT CALLBACK jojWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

JAPI ErrorCode platform_init(JPlatformManager* platform_manager, u16 width, u16 height, const char* title, WindowMode mode);
JAPI void platform_shutdown(JPlatformManager* platform_manager);
JAPI b8 platform_process_events(JPlatformManager* platform_manager);
JAPI void platform_set_window_title(JPlatformManager* platform_manager, const char* title);

JAPI void print_from_joj();

#endif // JOJ_PLATFORM_H