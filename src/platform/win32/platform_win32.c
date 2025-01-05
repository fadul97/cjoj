#include "platform/platform.h"

#if JPLATFORM_WINDOWS

#include <stdio.h>
#include <stdlib.h>

typedef struct Win32Window
{
    HWND handle;
    HDC hdc;
    WindowMode mode;
    u16 width;
    u16 height;
} JWindow;

typedef struct JPlatformManager_st
{
    JWindow* window;
} JPlatformManager;

JPlatformManager* g_platform_manager = NULL;
b8 g_plat_initialized = FALSE;

ErrorCode platform_init(u16 width, u16 height, const char* title, WindowMode mode)
{
    if (g_plat_initialized == TRUE) {
        printf("JPlatformManager was already initialized.\n");
        return OK;
    }

    if (g_platform_manager == NULL) {
        printf("JPlatformManager is NULL. Allocating memory...\n");
        g_platform_manager = (JPlatformManager*)malloc(sizeof(JPlatformManager));

        if (g_platform_manager == NULL) {
            printf("Failed to allocate memory for JPlatformManager.\n");
            return FAIL;
        }
    }

    g_platform_manager->window = (JWindow*)malloc(sizeof(JWindow));
    if (g_platform_manager->window == NULL) {
        printf("Failed to allocate memory for JWindow.\n");
        return FAIL;
    }

    JWindow* window = (JWindow*)g_platform_manager->window;
    if (window == NULL) {
        return FAIL;
    }

    const char* joj_wnd_class_name = "JOJ_WINDOW_CLASS";

    HINSTANCE app_id = GetModuleHandle(NULL);
    if (!app_id) {
        printf("Failed to get module handle.\n");
        return ERR_WIN32_WINDOW_GET_HANDLE;
    }

    WNDCLASSEX wnd_class;

    if (!GetClassInfoExA(app_id, joj_wnd_class_name, &wnd_class)) {
        wnd_class.cbSize = sizeof(WNDCLASSEX);
        wnd_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wnd_class.lpfnWndProc = jojWinProc;
        wnd_class.cbClsExtra = 0;
        wnd_class.cbWndExtra = 0;
        wnd_class.hInstance = app_id;
        wnd_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
        wnd_class.hbrBackground = CreateSolidBrush(RGB(60, 60, 60));
        wnd_class.lpszMenuName = NULL;
        wnd_class.lpszClassName = joj_wnd_class_name;
        wnd_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

        // Register "JOJ_WINDOW_CLASS" class
        if (!RegisterClassEx(&wnd_class)) {
            printf("Failed to register window class.\n");
            return ERR_WIN32_WINDOW_REGISTRATION;
        }
    }

    DWORD style = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;

    if (mode == WINDOWED) {
        i32 screen_width = GetSystemMetrics(SM_CXSCREEN);
        i32 screen_height = GetSystemMetrics(SM_CYSCREEN);

        if (width >= screen_width)
            window->width = screen_width;
        else
            window->width = width;

        if (height >= screen_height)
            window->height = screen_height;
        else
            window->height = height;
    }
    else if (mode == FULLSCREEN) {
        // Ignore width and height paremeters
        style = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
        window->width = GetSystemMetrics(SM_CXSCREEN);
        window->height = GetSystemMetrics(SM_CYSCREEN);
    }
    else {
        style = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
        window->width = width;
        window->height = height;
    }

    window->mode = mode;

    window->handle = CreateWindowEx(
        0,
        joj_wnd_class_name,
        title,
        style,
        0, 0,
        window->width, window->height,
        NULL,
        NULL,
        app_id,
        NULL
    );

    if (!window->handle) {
        printf("Failed to create window.\n");
        return ERR_WIN32_WINDOW_HANDLE_CREATION;
    }

    RECT new_rect = { 0, 0, window->width, window->height };
    if (window->mode == WINDOWED || window->mode == BORDERLESS) {
        if (!AdjustWindowRectEx(&new_rect,
            GetWindowStyle(window->handle),
            GetMenu(window->handle) != NULL,
            GetWindowExStyle(window->handle)))
        {
            printf("Could not adjust window rect ex.\n");
        }

        LONG xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - ((new_rect.right - new_rect.left) / 2);
        LONG ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - ((new_rect.bottom - new_rect.top) / 2);

        if (!MoveWindow(
            window->handle,
            xpos,
            ypos,
            new_rect.right - new_rect.left,
            new_rect.bottom - new_rect.top,
            TRUE))
        {
            printf("Could not move window.\n");
        }
    }

    window->hdc = GetDC(window->handle);
    if (!window->hdc) {
        printf("Failed to get device context.\n");
    }

    g_plat_initialized = TRUE;

    return OK;
}

void platform_shutdown()
{
    if (g_plat_initialized == FALSE) {
        printf("JPlatformManager NOT initialized.\n");
        return;
    }

    JWindow* window = (JWindow*)g_platform_manager->window;

    if (window->hdc != NULL) {
        ReleaseDC(window->handle, window->hdc);
        window->hdc = NULL;
    }

    if (window->handle != NULL) {
        DestroyWindow(window->handle);
        window->handle = NULL;
    }

    g_plat_initialized = FALSE;
}

b8 platform_process_events()
{
    if (g_plat_initialized == FALSE) {
        printf("JPlatformManager NOT initialized.\n");
        return FALSE;
    }

    MSG msg;

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
            return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK jojWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_plat_initialized == FALSE) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    switch (msg) {
    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void platform_set_window_title(const char* title)
{
    if (g_plat_initialized == FALSE) {
        printf("JPlatformManager NOT initialized.\n");
        return;
    }

    JWindow* window = (JWindow*)g_platform_manager->window;

    SetWindowText(window->handle, title);
}

void window_get_size(u16* width, u16* height)
{
    if (g_plat_initialized == FALSE) {
        printf("JPlatformManager NOT initialized.\n");
        return;
    }

    JWindow* window = (JWindow*)g_platform_manager->window;
    *width = window->width;
    *height = window->height;
}

void* platform_get_handle()
{
    if (g_plat_initialized == FALSE) {
        printf("JPlatformManager NOT initialized.\n");
        return;
    }

    JWindow* window = (JWindow*)g_platform_manager->window;
    return (void*)window->handle;
}

JAPI WindowMode platform_get_window_mode()
{
    if (g_plat_initialized == FALSE) {
        printf("JPlatformManager NOT initialized.\n");
        return;
    }

    JWindow* window = (JWindow*)g_platform_manager->window;
    return window->mode;
}

void print_from_joj()
{
    const i32 n = 10;
    printf("Print from jojPlatform! n = %d.\n", n);
}

#endif // JPLATFORM_WINDOWS