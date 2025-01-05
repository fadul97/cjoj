#ifndef JOJ_PLATFORM_H
#define JOJ_PLATFORM_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "error_code.h"

#include <Windows.h>
#include <windowsx.h>

typedef enum WindowMode_enum { BORDERLESS, FULLSCREEN, WINDOWED } WindowMode;

LRESULT CALLBACK jojWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

JAPI ErrorCode platform_init(u16 width, u16 height, const char* title, WindowMode mode);
JAPI void platform_shutdown();
JAPI b8 platform_process_events();
JAPI void platform_set_window_title(const char* title);

JAPI void window_get_size(u16* width, u16* height);
JAPI void* platform_get_handle();
JAPI WindowMode platform_get_window_mode();

JAPI void print_from_joj();

#endif // JOJ_PLATFORM_H