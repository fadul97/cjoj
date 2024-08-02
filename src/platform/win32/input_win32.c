#include "platform/input.h"

#if JPLATFORM_WINDOWS

#include "platform/keys.h"
#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>

typedef struct JInput_st
{
    Keyboard keyboard;
    Keyboard ctrl;
    Mouse mouse;
} JInput;

static b8 initialized = false;
static JInput input = { 0 };

LRESULT CALLBACK jojInputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void input_init()
{
    if (!initialized) {
        return;
    }

    for (i32 i = 0; i < 256; ++i) {
        input.keyboard.keys[i] = false;
        input.ctrl.keys[i] = false;
    }

    for (i32 i = 0; i < MAX_BUTTONS; ++i) {
        input.mouse.buttons[i] = false;
    }
}

void input_shutdown()
{
    initialized = false;
}

void input_set_window(struct JWindow* window)
{
    if (window == NULL) {
        return;
    }

    SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)jojInputProc);
}

b8 input_is_key_down(u32 key)
{
    return input.keyboard.keys[key];
}

b8 input_is_key_pressed(u32 key)
{
    if (input.ctrl.keys[key])
    {
        if (is_key_down(key))
        {
            input.ctrl.keys[key] = false;
            return true;
        }
    }
    else if (is_key_up(key))
    {
        input.ctrl.keys[key] = true;
    }

    return false;
}

b8 input_is_key_up(u32 key)
{
    return !input.keyboard.keys[key];
}

b8 input_is_button_down(Buttons button)
{
    return input.mouse.buttons[button];
}

b8 input_is_button_up(Buttons button)
{
    return !input.mouse.buttons[button];
}

i16 input_get_xmouse()
{
    return input.mouse.x;
}

i16 input_get_ymouse()
{
    return input.mouse.y;
}

i16 input_get_mouse_wheel()
{
    return input.mouse.wheel;
}

LRESULT CALLBACK jojInputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // Key pressed
    case WM_KEYDOWN:
        input.keyboard.keys[wParam] = true;
        return 0;

        // Key released
    case WM_KEYUP:
        input.keyboard.keys[wParam] = false;
        return 0;

        // Mouse movement
    case WM_MOUSEMOVE:
        input.mouse.x = GET_X_LPARAM(lParam);
        input.mouse.y = GET_Y_LPARAM(lParam);
        return 0;

        // Mouse wheel movement
    case WM_MOUSEWHEEL:
        input.mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
        return 0;

        // Left mouse button pressed
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        input.keyboard.keys[VK_LBUTTON] = true;
        input.mouse.buttons[BUTTON_LEFT] = true;
        return 0;

        // Middle mouse button pressed
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        input.keyboard.keys[VK_MBUTTON] = true;
        input.mouse.buttons[BUTTON_MIDDLE] = true;
        return 0;

        // Right mouse button pressed
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        input.keyboard.keys[VK_RBUTTON] = true;
        input.mouse.buttons[BUTTON_RIGHT] = true;
        return 0;

        // Left mouse button released
    case WM_LBUTTONUP:
        input.keyboard.keys[VK_LBUTTON] = false;
        input.mouse.buttons[BUTTON_LEFT] = false;
        return 0;

        // Middle mouse button released
    case WM_MBUTTONUP:
        input.keyboard.keys[VK_MBUTTON] = false;
        input.mouse.buttons[BUTTON_MIDDLE] = false;
        return 0;

        // Right mouse button released
    case WM_RBUTTONUP:
        input.keyboard.keys[VK_RBUTTON] = false;
        input.mouse.buttons[BUTTON_RIGHT] = false;
        return 0;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
        //return CallWindowProc(jojWinProc, hWnd, msg, wParam, lParam);
    }
}

#endif // JPLATFORM_WINDOWS