#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "ui.h"
#include "gpib.h"

// plus tard: #include "gpib_ctrl.h"

int main(int argc, char **argv)
{

    pthread_t gpib_thread;

    // Lancer le thread de polling AVANT gtk_main
    if (pthread_create(&gpib_thread, NULL, gpib_poll_thread, NULL) != 0) {
        fprintf(stderr, "Failed to start GPIB polling thread\n");
        return EXIT_FAILURE;
    }

    if (hmi_init(&argc, &argv) < 0) {
        fprintf(stderr, "Failed to run UI\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

