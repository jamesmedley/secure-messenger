#include <gtk/gtk.h>
#include "gui/gui.h"
#include "encryption/rsa1.h"


int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    app = gtk_application_new("com.example.messenger", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}