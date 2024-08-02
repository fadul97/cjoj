#ifndef JOJ_INPUT_H
#define JOJ_INPUT_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "keys.h"
#include "platform.h"

struct JInput;

void input_init();
void input_shutdown();

void input_set_window(struct JWindow* window);

b8 input_is_key_down(u32 key);
b8 input_is_key_pressed(u32 key);
b8 input_is_key_up(u32 key);

b8 input_is_button_down(Buttons button);
b8 input_is_button_up(Buttons button);

i16 input_get_xmouse();
i16 input_get_ymouse();
i16 input_get_mouse_wheel();

#endif // JOJ_INPUT_H