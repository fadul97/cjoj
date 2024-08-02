// main.c : Defines the entry point for the application.
//

#include <stdio.h>
#include "joj/error_code.h"
#include "joj/platform/platform.h"

int main()
{
    JPlatformManager plat;
    if (platform_init(&plat, 800, 600, "Joj Engine", WINDOWED) != OK) {
        printf("Failed to initialize Platform.");
        return FAIL;
    }

    b8 running = true;
    while (running) {
        if (!platform_process_events(&plat))
            running = false;
    }

    print_from_joj();
	return 0;
}
