#include <gtk/gtk.h>
#include "ui.h"
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

/* ------------------------------------------------------------------ */
/* Timer de mise à jour UI                                            */
/* ------------------------------------------------------------------ */



gboolean ui_update_labels(gpointer user_data)
{
    (void)user_data;

    /* TODO: adapter avec ta structure g_controller ou autres sources
     * Exemple:
     *
     * char buf[64];
     * snprintf(buf, sizeof(buf), "Emitter Temp. : %.2f C", g_controller.emitter_temp);
     * gtk_label_set_text(GTK_LABEL(label_emitter_temp), buf);
     *
     * idem pour label_target_temp, label_target_index, label_differential_temp,
     * label_setpoint_ready, label_mrtd_progress, label_dev_status.
     */

    return TRUE; /* continuer le timeout */
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
    /* TODO: basculer stack1 vers la page1 en mode MANUAL */
    /* gtk_stack_set_visible_child_name(GTK_STACK(stack1), "page1"); */
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
    (void)button;
    (void)user_data;

    /* TODO: init GPIB + mise à jour label_dev_status */
    /* int ret = gpib_init(master_addr, dev_addr); */
    /* if (ret == 0) gtk_label_set_text(GTK_LABEL(label_dev_status), " ONLINE "); */
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
    /* TODO: demander confirmation puis reboot raspi (system("sudo reboot") ?) */
}

void on_btn_shutdown_raspi_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* TODO: confirmation puis shutdown (system("sudo poweroff") ?) */
}

/* ------------------------------------------------------------------ */
/* Callbacks page MRTD / MANUAL (page1)                               */
/* ------------------------------------------------------------------ */

void on_btn_back_menu_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;
    /* Retour au menu principal (page0) */
    /* gtk_stack_set_visible_child_name(GTK_STACK(stack1), "page0"); */
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

int hmi_init(int *argc, char ***argv)
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

    /* Labels status/temp/MRTD */
    label_differential_temp = GTK_WIDGET(gtk_builder_get_object(builder, "label_differential_temp"));
    label_setpoint_ready    = GTK_WIDGET(gtk_builder_get_object(builder, "label_setpoint_ready"));
    label_mrtd_progress     = GTK_WIDGET(gtk_builder_get_object(builder, "label_mrtd_progress"));
    label_emitter_temp      = GTK_WIDGET(gtk_builder_get_object(builder, "label_emitter_temp"));
    label_target_temp       = GTK_WIDGET(gtk_builder_get_object(builder, "label_target_temp"));
    label_target_index      = GTK_WIDGET(gtk_builder_get_object(builder, "label_target_index"));
    label_dev_status        = GTK_WIDGET(gtk_builder_get_object(builder, "label_dev_status"));

    /* Signaux automatiques (basés sur handler="..." dans le .glade) */
    gtk_builder_connect_signals(builder, NULL);

    /* Signaux génériques */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    g_object_unref(builder);

    /* Timer de mise à jour des labels */
    g_timeout_add(500, ui_update_labels, NULL);

    gtk_main();
    return 0;
}