#include <gtk/gtk.h>
#include "core.h"
#include "gpib.h"      

#define master_addr 0
#define dev_addr    1



static GtkWidget *stack1;

static GtkWidget *txtView_menu;
static GtkWidget *txtView_manual_log;


static GtkWidget *label_differential_temp;
static GtkWidget *label_setpoint_ready;
static GtkWidget *label_mrtd_progress;
static GtkWidget *label_emitter_temp;
static GtkWidget *label_target_temp;
static GtkWidget *label_target_index;
static GtkWidget *label_dev_status;
static GtkWidget *btn_auto;
static GtkWidget *btn_manual;
static GtkWidget *btn_help;
static GtkWidget *btn_connect_dev;
static GtkWidget *btn_tgt_table;
static GtkWidget *btn_serial_log;
static GtkWidget *btn_increase_temp;
static GtkWidget *btn_decrease_temp;
static GtkWidget *btn_save_mrtd_mesure;
static GtkWidget *btn_show_table;
static GtkWidget *btn_show_graph;
static GtkWidget *btn_reset_data;
static GtkWidget *btn_undo_last_mesure;
static GtkWidget *btn_invert_d;
static GtkWidget *btn_back_menu;
ProgramMode mode = MENU;
/* ------------------------------------------------------------------ */
/* !!! GTK N'EST PAS THREAD-SAFE !!! 
Nous ne pouvons pas appeler les fonction GTK depuis un autre thread, 
Nous ne pouvons pas non plus faire du polling dans le thread GTK, cela rendrait l'interface non réactive et pourrait causer des blocages.
Nous ne pouvons pas mettre a jour un label comme gtk_label_set_text() depuis le thread de polling, cela causerait des comportements indéterminés et potentiellement des crashs.

TODO: mettre en place un systeme de queu pour transmettre les actions a executer par le thread gpib. 
*/

/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/* Timer de mise à jour UI                                            */
/* ------------------------------------------------------------------ */

typedef enum {
    UI_LOCK,
    UI_UNLOCK
} UiLockState;

void ui_sensitive(UiLockState ui_state)
{
    gtk_widget_set_sensitive(btn_manual, ui_state);
    gtk_widget_set_sensitive(btn_auto, ui_state);
    gtk_widget_set_sensitive(btn_tgt_table, ui_state);
    gtk_widget_set_sensitive(btn_serial_log, ui_state);
   /* gtk_widget_set_sensitive(btn_increase_temp, ui_state);
    gtk_widget_set_sensitive(btn_decrease_temp, ui_state);
    gtk_widget_set_sensitive(btn_save_mrtd_mesure, ui_state);
    gtk_widget_set_sensitive(btn_show_table, ui_state);
    gtk_widget_set_sensitive(btn_show_graph, ui_state);
    gtk_widget_set_sensitive(btn_reset_data, ui_state);
    gtk_widget_set_sensitive(btn_undo_last_mesure, ui_state);
    gtk_widget_set_sensitive(btn_invert_d, ui_state);*/
}


/*
    Exécuté toute les 500ms.
*/
gboolean ui_update_labels(gpointer user_data)
{
    AppData *app = (AppData *)user_data;
    GpibData snap;
    char     buf[64];

    if (app == NULL) return TRUE;

    pthread_mutex_lock(&app->mutex);
    snap = app->device_status;
    pthread_mutex_unlock(&app->mutex);

    /* Delta T */
    snprintf(buf, sizeof(buf), "ΔT = %.3f °C", snap.actual_dt);
    gtk_label_set_text(GTK_LABEL(label_differential_temp), buf);

    /* Température émetteur */
    snprintf(buf, sizeof(buf), "Emitter : %.2f °C", snap.emitter_temp);
    gtk_label_set_text(GTK_LABEL(label_emitter_temp), buf);

    /* Température cible */
    snprintf(buf, sizeof(buf), "Target : %.2f °C", snap.target_temp);
    gtk_label_set_text(GTK_LABEL(label_target_temp), buf);

    /* Index roue */
    snprintf(buf, sizeof(buf), "Target index :%d/12", snap.target_index);
    gtk_label_set_text(GTK_LABEL(label_target_index), buf);

    return TRUE;
}

gboolean set_status_online(gpointer data)
{
    (void)data;
    gtk_label_set_text(GTK_LABEL(label_dev_status), "ONLINE");
    return FALSE;
}

gboolean set_status_offline(gpointer data)
{
    (void)data;
    gtk_label_set_text(GTK_LABEL(label_dev_status), "OFFLINE");
    return FALSE;
}

void hmi_log_append(const char *text)
{
    if (!txtView_menu) return;
    if (!txtView_manual_log) return;

    // Choisir la vue cible selon le mode
    GtkWidget *target_view;

    if (mode == MANUAL) {
        target_view = txtView_manual_log;
    } else {
        target_view = txtView_menu;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(target_view));
    GtkTextIter end;

    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, text, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);

    GtkTextMark *mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(target_view), mark, 0.0, FALSE, 0.0, 1.0);
}


// Cette fonction sera exécutée par le thread GTK (UI) mais appelée depuis n'importe quel thread via g_idle_add() pour garantir la sécurité des threads.
gboolean hmi_log_append_idle(gpointer data) 
{
    char *text = (char *)data;


    if (text != NULL) {
        hmi_log_append(text); // Appelle ta fonction d'origine
        free(text);           // Libère la chaîne allouée par le thread service
    }

    return FALSE; // Indique à GTK de ne pas réexécuter cette fonction en boucle
}
/* ------------------------------------------------------------------ */
/* Callbacks MENU principal (page0)                                   */
/* ------------------------------------------------------------------ */

void on_btn_auto_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: basculer stack1 vers la page1 en mode AUTO, init séquence auto */
    /* gtk_stack_set_visible_child_name(GTK_STACK(stack1), "page1"); */
}

void on_btn_manual_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;

    AppData *app = (AppData *)user_data;

    if (app == NULL) return;

    if(mode != MENU){
        return;                  //Ne dois jamais éxécuter cette ligne
    } else {
        mode = MANUAL;
    }

    app_set_service_gpib(app, COMMUNICATION);
    hmi_log_append("Mode manuel demandé...");

    gtk_stack_set_visible_child_name(GTK_STACK(stack1), "page1");
}

void on_btn_tgt_table_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: ouvrir / afficher table des cibles (fenêtre ou autre stack) */
}

void on_btn_help_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: afficher fenêtre d'aide / message dialog */
}

/* ------------------------------------------------------------------ */
/* Callbacks colonne droite (GPIB / système)                          */
/* ------------------------------------------------------------------ */

void on_btn_connect_dev_clicked(GtkButton *button, gpointer user_data)
{
    (void)button; // Pour éviter le warning "unused parameter"

    // RÉCUPÉRATION DU POINTEUR :
    AppData *app = (AppData *)user_data;

    if (app == NULL) return; // Sécurité

    app_set_service_gpib(app, CONNECT);
    hmi_log_append("Ordre de connexion envoyé...");
    
}

void on_btn_serial_log_toggled(GtkToggleButton *button, gpointer user_data)
{
    (void)user_data;
    gboolean active = gtk_toggle_button_get_active(button);

    /* TODO: activer/désactiver log série, afficher info dans txtView_menu */
    if (active) {
        /* log ON */
    } else {
        /* log OFF */
    }
}

void on_btn_rst_raspi_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    
    printf("Redémarrage du système...");
    hmi_log_append("Redémarrage du système...");
    //cleanup_and_quit();
    hmi_log_append("GPIB libéré proprement");
    gtk_main_quit();
    //g_spawn_command_line_async("sudo reboot", NULL);
}

void on_btn_shutdown_raspi_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    
    printf("Extinction du système...");
    hmi_log_append("Extinction du système...");
    //cleanup_and_quit();
    hmi_log_append("GPIB libéré proprement");
    gtk_main_quit();
    //n'est pas exécuté g_spawn_command_line_async("sudo shutdown now", NULL);
}

/* ------------------------------------------------------------------ */
/* Callbacks page MRTD / MANUAL (page1)                               */
/* ------------------------------------------------------------------ */

void on_btn_back_menu_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* Retour au menu principal (page0) */

        // RÉCUPÉRATION DU POINTEUR :
    AppData *app = (AppData *)user_data;

    if (app == NULL) return; // Sécurité

    if(mode == MENU){
        hmi_log_append("ERROR: Impossible de retourner\nau menu.");
        return;                       //Ne dois jamais éxécuter cette ligne
    } else {
        mode = MENU;
    }

    app_set_service_gpib(app, IDLE);
    gtk_stack_set_visible_child_name(GTK_STACK(stack1), "page0");
}

void on_btn_increase_pressed(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: commencer incrémentation température (appui long ?) */
    /* ex: gpib_temp_inc_start(); */
}

void on_btn_increase_released(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: arrêter incrémentation température */
    /* ex: gpib_temp_inc_stop(); */
}

void on_btn_decrease_temp_pressed(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: commencer décrémentation température */
}

void on_btn_decrease_temp_released(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: arrêter décrémentation température */
}

void on_btn_save_mrtd_mesure_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: sauver point MRTD courant, mettre à jour label_mrtd_progress */
}

void on_btn_show_table_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: afficher table MRTD */
}

void on_btn_show_graph_clicked(GtkButton *button, gpointer user_data){
    (void)button;
    (void)user_data;
}

void on_btn_reset_data_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: reset data MRTD + refresh labels/graph */
}

void on_btn_undo_last_mesure_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: annuler dernier point MRTD */
}

void on_btn_invert_d_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: inverser signe de d (deltaT ?) et mettre à jour affichage */
}

/* ------------------------------------------------------------------ */
/* Initialisation HMI                                                 */
/* ------------------------------------------------------------------ */

int hmi_init(int *argc, char ***argv, AppData *app)
{
    GtkBuilder *builder;
    GtkWidget  *window;

    gtk_init(argc, argv);

    builder = gtk_builder_new_from_file(GLADE_PATH);

    /* Fenêtre principale */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "hWindows"));

    /* Stack principal */
    stack1 = GTK_WIDGET(gtk_builder_get_object(builder, "stack1"));

    /* Widgets texte/log */
    txtView_menu = GTK_WIDGET(gtk_builder_get_object(builder, "txtView_menu"));
    txtView_manual_log = GTK_WIDGET(gtk_builder_get_object(builder, "txtView_manual_log"));

    /* Labels status/temp/MRTD */
    label_differential_temp = GTK_WIDGET(gtk_builder_get_object(builder, "label_differential_temp"));
    label_setpoint_ready    = GTK_WIDGET(gtk_builder_get_object(builder, "label_setpoint_ready"));
    label_mrtd_progress     = GTK_WIDGET(gtk_builder_get_object(builder, "label_mrtd_progress"));
    label_emitter_temp      = GTK_WIDGET(gtk_builder_get_object(builder, "label_emitter_temp"));
    label_target_temp       = GTK_WIDGET(gtk_builder_get_object(builder, "label_target_temp"));
    label_target_index      = GTK_WIDGET(gtk_builder_get_object(builder, "label_target_index"));
    label_dev_status        = GTK_WIDGET(gtk_builder_get_object(builder, "label_dev_status"));

    /* Boutons */
    btn_connect_dev      = GTK_WIDGET(gtk_builder_get_object(builder, "btn_connect_dev"));
    btn_manual           = GTK_WIDGET(gtk_builder_get_object(builder, "btn_manual"));
    btn_auto             = GTK_WIDGET(gtk_builder_get_object(builder, "btn_auto"));
    btn_help             = GTK_WIDGET(gtk_builder_get_object(builder, "btn_help"));
    btn_tgt_table        = GTK_WIDGET(gtk_builder_get_object(builder, "btn_tgt_table"));
    btn_serial_log       = GTK_WIDGET(gtk_builder_get_object(builder, "btn_serial_log"));
    btn_increase_temp    = GTK_WIDGET(gtk_builder_get_object(builder, "btn_increase_temp"));
    btn_decrease_temp    = GTK_WIDGET(gtk_builder_get_object(builder, "btn_decrease_temp"));
    btn_save_mrtd_mesure = GTK_WIDGET(gtk_builder_get_object(builder, "btn_save_mrtd_mesure"));
    btn_show_table       = GTK_WIDGET(gtk_builder_get_object(builder, "btn_show_table"));
    btn_show_graph       = GTK_WIDGET(gtk_builder_get_object(builder, "btn_show_graph"));
    btn_reset_data       = GTK_WIDGET(gtk_builder_get_object(builder, "btn_reset_data"));
    btn_undo_last_mesure = GTK_WIDGET(gtk_builder_get_object(builder, "btn_undo_last_mesure"));
    btn_invert_d         = GTK_WIDGET(gtk_builder_get_object(builder, "btn_invert_d"));
    btn_back_menu        = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_menu"));
    /* Signaux automatiques (basés sur handler="..." dans le .glade) */
    gtk_builder_connect_signals(builder, NULL);

    /* Signaux génériques */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_connect_dev, "clicked", G_CALLBACK(on_btn_connect_dev_clicked), app);
    g_signal_connect(btn_manual, "clicked", G_CALLBACK(on_btn_manual_clicked), app);
    g_signal_connect(btn_auto, "clicked", G_CALLBACK(on_btn_auto_clicked), app);
    g_signal_connect(btn_back_menu, "clicked", G_CALLBACK(on_btn_back_menu_clicked), app);


    gtk_widget_show_all(window);
    g_object_unref(builder);  /* ← après tous les gtk_builder_get_object */

    /* Timer de mise à jour des labels */
    g_timeout_add(500, ui_update_labels, app);

    gtk_main();
    return 0;
}