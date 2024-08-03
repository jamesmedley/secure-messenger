#include <gtk/gtk.h>
#include "gui/gui.h"
#include "networking/networking.h"
#include <windows.h>

int main(int argc, char **argv) {

/*     setupSocket();
    char dest_ip[INET_ADDRSTRLEN];
    char message[BUFFER_SIZE];

    printf("Enter destination IP address: ");
    scanf("%s", dest_ip);
    printf("Enter message: ");
    getchar(); // Clear the newline left by scanf
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove the newline character

    if (sendMessage(message, dest_ip) == 0) {
        printf("Message sent successfully\n");
    } else {
        printf("Failed to send message\n");
    }
    Sleep(1000);

    return 0; */

    GtkApplication *app;
    int status;
    app = gtk_application_new("com.example.messenger", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
