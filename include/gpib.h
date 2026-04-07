#ifndef GPIB_H
#define GPIB_H
#include <gtk/gtk.h>
#include <pthread.h>

typedef enum {
    MASTER_OFFLINE_DEVICE_OFFLINE = 0,
    MASTER_ONLINE_DEVICE_OFFLINE,
    MASTER_ONLINE_DEVICE_ONLINE,
    //DEVICE_ERROR
} DeviceState;

typedef struct {
    double target_temp;
    double emitter_temp;
    int    target_index;
    int    last_error;
    DeviceState state;
    int    ud;
} ControllerState;

extern ControllerState g_controller;

void *gpib_poll_thread(void *arg);
DeviceState gpib_init(int master_addr, int dev_addr);
int gpib_close(void);
int gpib_temp_inc(void);
int gpib_temp_dec(void);
int gpib_next_target(void);
int gpib_read_all(void);
int gpib_write(const char *command);
int gpib_read(char *response);
int gpib_write_read(const char *command, char *response);
void cleanup_and_quit(void);







typedef struct
{
    double target_temp;     //Une structure de transition est necessaire pour transmettre les donnees a la structure generale ControllerState,
    double emitter_temp;    //sans cela a chaque fois que le thread de polling lit les donnes gpib il aura la main constante sur le mutex global
    int    target_index;    //Ce qui n'est pas acceptable pour la reactivite de l'UI et un danger potentiel pour la stabilité de l'application (risque de deadlock)
    DeviceState state;
    int    ud;
} GpibLocalData;

extern GpibLocalData local_controller;

#endif 