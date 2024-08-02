// main.c : Defines the entry point for the application.
//

#include <stdio.h>
#include "joj/defines.h"
#include "joj/error_code.h"
#include "joj/platform/platform.h"

#include <Windows.h>
#include <windowsx.h>

typedef enum WindowMode_enum { BORDERLESS, FULLSCREEN, WINDOWED } WindowMode;

typedef struct Win32Window
{
    HWND handle;
    HDC hdc;
    WindowMode mode;
    u16 width;
    u16 height;
} Window;

ErrorCode platform_init(Window* window, u16 width, u16 height, const char* title, WindowMode mode);
void platform_shutdown(Window* window);
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
    Window window;
    if (platform_init(&window, 800, 600, "Joj Engine", WINDOWED) != OK) {
        printf("Failed to initialize Platform.");
        return FAIL;
    }

    b8 running = true;
    while (running) {
        MSG msg;

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
                running = false;
        }
    }

    print_from_joj();
	return 0;
}

ErrorCode platform_init(Window* window, u16 width, u16 height, const char* title, WindowMode mode)
{
    const char* joj_wnd_class_name = "JOJ_WINDOW_CLASS";

    HINSTANCE app_id = GetModuleHandle(NULL);
    if (!app_id) {
        printf("Failed to get module handle.");
        return ERR_WIN32_WINDOW_GET_HANDLE;
    }

    WNDCLASSEX wnd_class;

    if (!GetClassInfoExA(app_id, joj_wnd_class_name, &wnd_class)) {
        wnd_class.cbSize = sizeof(WNDCLASSEX);
        wnd_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wnd_class.lpfnWndProc = WinProc;
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
            printf("Failed to register window class.");
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
        printf("Failed to create window.");
        return ERR_WIN32_WINDOW_HANDLE_CREATION;
    }

    RECT new_rect = { 0, 0, window->width, window->height };
    if (window->mode == WINDOWED || window->mode == BORDERLESS) {
        if (!AdjustWindowRectEx(&new_rect,
            GetWindowStyle(window->handle),
            GetMenu(window->handle) != NULL,
            GetWindowExStyle(window->handle)))
        {
            printf("Could not adjust window rect ex.");
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
            printf("Could not move window.");
        }
    }

    window->hdc = GetDC(window->handle);
    if (!window->hdc) {
        printf("Failed to get device context.");
    }

    return OK;
}

void platform_shutdown(Window* window)
{
    if (window->hdc != NULL) {
        ReleaseDC(window->handle, window->hdc);
        window->hdc = NULL;
    }

    if (window->handle != NULL) {
        DestroyWindow(window->handle);
        window->handle = NULL;
    }
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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