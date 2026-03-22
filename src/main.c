#include <stdlib.h>
#include <stdio.h>
#include "ui.h"
#ifdef PLATFORM_LINUX
#include "gpib.h"
#endif
// plus tard: #include "gpib_ctrl.h"

int main(int argc, char **argv)
{

    #ifdef PLATFORM_LINUX
        if (gpib_init(1) < 0) {
            fprintf(stderr, "Failed to initialize GPIB\n");
            return EXIT_FAILURE;
        }
    #endif

        if (hmi_init(&argc, &argv) < 0) {
            fprintf(stderr, "Failed to run UI\n");
            return EXIT_FAILURE;
        }
    #ifdef PLATFORM_LINUX
        gpib_close();
    #endif
        return EXIT_SUCCESS;

}

