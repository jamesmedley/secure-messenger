#include <gtk/gtk.h>

// Callback function for sending a message
void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *scrollable;
    GtkWidget *text_view;
    GtkWidget *entry;
    GtkWidget *send_button;
    GtkTextBuffer *buffer;

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Messenger");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a vertical box layout
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Create a scrollable text view for displaying messages
    scrollable = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(box), scrollable);
    
    text_view = gtk_text_view_new();
    gtk_widget_set_vexpand(text_view, TRUE);
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollable), text_view);
    
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, "Welcome to the Messenger!", -1);

    // Create an entry widget for input
    entry = gtk_entry_new();
    gtk_box_append(GTK_BOX(box), entry);

    // Create a send button
    send_button = gtk_button_new_with_label("Send");
    gtk_box_append(GTK_BOX(box), send_button);

    // Connect the send button's clicked signal to the callback function
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), text_view);

    // Connect the entry to send message on Enter key press
    g_signal_connect(entry, "activate", G_CALLBACK(on_send_button_clicked), text_view);

    // Present the window
    gtk_window_present(GTK_WINDOW(window));
}