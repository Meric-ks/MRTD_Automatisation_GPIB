#ifndef CORE_H
#define CORE_H
#include <pthread.h> 
#include <stdbool.h>
#include <gtk/gtk.h>


typedef enum
{
    MENU,
    MANUAL,
    AUTO
} ProgramMode;

typedef enum
{
    IDLE,
    CONNECT,
    COMMUNICATION,
    SHUTDOWN
} ServiceGpib;

// Structure pour les données brutes du matériel
typedef struct {
    int    ud;            // Unit Descriptor GPIB
    double actual_dt;      // Delta entre target et emitter
    double target_temp;   // Température cible
    double emitter_temp;  // Température réelle
    int    target_index;  // Position de la roue
} GpibData;

// Structure de contexte global pour l'application
typedef struct {
    ServiceGpib     service_gpib;      // Partagé entre thread service et watchdog
    bool            shutdown_requested;
    bool            device_online;
    pthread_mutex_t mutex;              // Protection des accès [cite: 14, 15]
    pthread_cond_t  cond;               // Signal de synchronisation [cite: 2, 7]
    GpibData        device_status;      // Données de mesure
} AppData;

void app_set_service_gpib(AppData *app, ServiceGpib service);
gboolean hmi_log_append_idle(gpointer data);
gboolean set_status_online(gpointer data);
gboolean set_status_offline(gpointer data);
int hmi_init(int *argc, char ***argv, AppData *app);
void* thread_service_gpib(void* arg);
void* thread_handler_watchdog(void* arg);

#endif
