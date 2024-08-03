#include <gtk/gtk.h>
#include "gui.h"

void on_send_message(GtkWidget *entry, gpointer user_data) {
    GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter end_iter;
    GtkTextMark *mark;
    
    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (g_strcmp0(text, "") != 0) { // Check if the entry is not empty
        gtk_text_buffer_get_end_iter(buffer, &end_iter);
        gtk_text_buffer_insert(buffer, &end_iter, text, -1);
        gtk_text_buffer_insert(buffer, &end_iter, "\n", -1); // Add a newline after the message
        
        gtk_editable_set_text(GTK_EDITABLE(entry), ""); // Clear the entry widget
        
        // Scroll to the end of the text buffer
        mark = gtk_text_buffer_create_mark(buffer, NULL, &end_iter, FALSE);
        gtk_text_view_scroll_mark_onscreen(text_view, mark);
        gtk_text_buffer_delete_mark(buffer, mark);
    }
}

void on_set_ip(GtkWidget *button_ip, gpointer user_data){
    IpData *data = (IpData *)user_data;
    GtkEntry *entry_ip = data->entry_ip;
    GtkLabel *label_ip = data->label_ip;

    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry_ip));
    if (g_strcmp0(text, "") != 0) { // TODO: verify string is valid IP address
        const gchar *prepend_text = "Chatting to: ";
        gsize prepend_text_len = strlen(prepend_text);
        gsize existing_text_len = strlen(text);
        gsize new_text_len = prepend_text_len + existing_text_len;
        gchar *new_text = g_malloc(new_text_len + 1);  // +1 for null terminator
        strcpy(new_text, prepend_text);
        strcat(new_text, text);

        gtk_label_set_text(label_ip, new_text);

        g_free(new_text);
    }
}


void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *box_ip; 
    GtkWidget *scrollable;
    GtkWidget *button_ip;
    GtkWidget *entry_ip;
    GtkWidget *label_ip;        
    GtkWidget *text_view;
    GtkWidget *entry_message;
    GtkTextBuffer *buffer;

    // window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Messenger");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // box containing: chat text_view, message entry
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // box containing: ip entry, set ip button
    box_ip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(box), box_ip);

    // ip entry
    entry_ip = gtk_entry_new();
    gtk_box_append(GTK_BOX(box_ip), entry_ip);

    // set ip button
    button_ip = gtk_button_new_with_label("Set Destination IP");
    gtk_box_append(GTK_BOX(box_ip), button_ip);

    // ip info label
    label_ip = gtk_label_new("Chatting to: ");
    gtk_box_append(GTK_BOX(box_ip), label_ip);

    IpData *ip_data = g_malloc(sizeof(IpData));
    ip_data->entry_ip = GTK_ENTRY(entry_ip);
    ip_data->label_ip = GTK_LABEL(label_ip);

    g_signal_connect(button_ip, "clicked", G_CALLBACK(on_set_ip), ip_data);

    // scrollable for text_view
    scrollable = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(box), scrollable);
    
    // messages text_view
    text_view = gtk_text_view_new();
    gtk_widget_set_vexpand(text_view, TRUE);
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollable), text_view);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);

    // messages buffer
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, "Welcome to the Messenger!\n", -1);

    // message entry
    entry_message = gtk_entry_new();
    gtk_box_append(GTK_BOX(box), entry_message);

    // send message callback 
    g_signal_connect(entry_message, "activate", G_CALLBACK(on_send_message), text_view);

    // Present the window
    gtk_window_present(GTK_WINDOW(window));
}