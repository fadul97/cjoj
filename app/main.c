// main.c : Defines the entry point for the application.
//

#include <stdio.h>
#include "joj/platform/platform.h"
#include "joj/platform/input.h"

int main()
{
    JPlatformManager plat;
    if (platform_init(&plat, 800, 600, "Joj Engine", WINDOWED) != OK) {
        printf("Failed to initialize Platform.");
        return FAIL;
    }

    input_init();

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
    }

    input_shutdown();
    platform_shutdown(&plat);

    print_from_joj();
	return 0;
}
