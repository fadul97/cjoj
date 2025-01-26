// main.c : Defines the entry point for the application.
//

#include <stdio.h>
#include "joj/platform/platform.h"
#include "joj/platform/input.h"
#include "joj/platform/timer.h"
#include "joj/renderer/renderer.h"

f32 frametime = 0.0f;
f32 get_frametime();

int main()
{
    if (platform_init(800, 600, "Joj Engine", WINDOWED) != OK) {
        printf("Failed to initialize Platform.");
        return FAIL;
    }

    input_init();

    if (renderer_init() != OK) {
        printf("Failed to initialize Renderer!\n");
        return -1;
    }

    time_create();
    time_start();
    time_begin_period();

    b8 running = TRUE;
    while (running) {
        f32 ft = get_frametime();

        if (!platform_process_events())
            running = FALSE;

        if (input_is_key_pressed(KEY_ESCAPE)) {
            running = FALSE;
        }

        if (input_is_key_pressed('A')) {
            printf("A pressed.\n");
        }

        if (input_is_key_down(KEY_SPACE)) {
            printf("Space down.\n");
        }

        renderer_start_frame(0.0f, 0.0f, 1.0f, 1.0f);
        renderer_end_frame();
    }

    time_end_period();
    input_shutdown();

    renderer_print();
    renderer_shutdown();

    platform_shutdown();

    return 0;
}

f32 get_frametime()
{
#ifdef _DEBUG
    static f32 total_time = 0.0f;	// Total time elapsed
    static u32  frame_count = 0;	// Elapsed frame counter
#endif

    // Current frame time
    frametime = (f32)time_reset();

#ifdef _DEBUG
    // Accumulated frametime
    total_time += frametime;

    // Increment frame counter
    frame_count++;

    // Updates FPS indicator in the window every 1000ms (1 second)
    if (total_time >= 1.0f)
    {
        char text[256];

        snprintf(text, sizeof(text), "Joj Engine    FPS: %d    Frametime: %.3f (ms)",
            frame_count, frametime * 1000);

        platform_set_window_title(text);

        frame_count = 0;
        total_time -= 1.0f;
    }
#endif

    return frametime;
}
