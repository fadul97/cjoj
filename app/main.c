// main.c : Defines the entry point for the application.
//

#include <stdio.h>
#include "joj/platform/platform.h"
#include "joj/platform/input.h"
#include "joj/platform/timer.h"

f32 frametime = 0.0f;
f32 get_frametime(JPlatformManager* plat);

int main()
{
    JPlatformManager plat;
    if (platform_init(&plat, 800, 600, "Joj Engine", WINDOWED) != OK) {
        printf("Failed to initialize Platform.");
        return FAIL;
    }

    input_init();

    time_create();
    time_start();
    time_begin_period();

    b8 running = true;
    while (running) {
        if (!platform_process_events(&plat))
            running = false;

        if (input_is_key_pressed(KEY_ESCAPE)) {
            running = false;
        }

        if (input_is_key_pressed('A')) {
            printf("A pressed.\n");
        }

        if (input_is_key_down(KEY_SPACE)) {
            printf("Space down.\n");
        }

        f32 ft = get_frametime(&plat);
        printf("ft = %f\n", ft);
    }

    time_end_period();
    input_shutdown();
    platform_shutdown(&plat);

    print_from_joj();
	return 0;
}

f32 get_frametime(JPlatformManager* plat)
{
#ifdef _DEBUG
    static f32 total_time = 0.0f;	// Total time elapsed
    static u32  frame_count = 0;	// Elapsed frame counter
#endif

    // Current frame time
    frametime = time_reset();

#ifdef _DEBUG
    // Accumulated frametime
    total_time += frametime;

    // Increment frame counter
    frame_count++;

    // Updates FPS indicator in the window every 1000ms (1 second)
    if (total_time >= 1.0f)
    {
        char text[256];

        printf("%f\n", frametime);

        snprintf(text, sizeof(text), "Joj Engine    FPS: %d    Frametime: %.3f (ms)",
            frame_count, frametime * 1000);

        platform_set_window_title(plat, text);

        frame_count = 0;
        total_time -= 1.0f;
    }
#endif

    return frametime;
}
