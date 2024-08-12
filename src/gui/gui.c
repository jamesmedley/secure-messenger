#include <gtk/gtk.h>
#include "gui.h"
#include "gui_utils.h"
#include "../networking/networking.h"
#include "../networking/tls_handler.h"

gchar *destination_ip = "127.0.0.1"; // localhost by default

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
GAsyncQueue *message_queue;

gboolean process_message_queue(gpointer user_data) {
    MessageQueueData *msg_data;
    
    while ((msg_data = g_async_queue_try_pop(message_queue)) != NULL) {
        write_message(msg_data->name, msg_data->message, buffer, GTK_TEXT_VIEW(text_view));

        g_free(msg_data->name);
        g_free(msg_data->message);
        g_free(msg_data);
    }

    return G_SOURCE_REMOVE; 
}

// Function to push a message onto the queue
void push_message_to_queue(const gchar *name, const gchar *message) {
    MessageQueueData *msg_data = g_new(MessageQueueData, 1);
    msg_data->name = g_strdup(name);
    msg_data->message = g_strdup(message);

    g_async_queue_push(message_queue, msg_data);
    
    g_idle_add(process_message_queue, NULL);
}

void on_display_message(GtkWidget *entry, gpointer user_data) {
    MessageData *data = (MessageData *)user_data;
    gchar *name = data->name;
        
    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (g_strcmp0(text, "") != 0) { // Check if the entry is not empty

        send_message(text, strlen(text), destination_ip);
        push_message_to_queue(name, text);
        gtk_editable_set_text(GTK_EDITABLE(entry), ""); // Clear the entry widget
    }
}

void on_set_ip(GtkWidget *button_ip, gpointer user_data){
    IpData *data = (IpData *)user_data;
    GtkEntry *entry_ip = data->entry_ip;
    GtkLabel *label_ip = data->label_ip;

    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry_ip));
    if (g_strcmp0(text, "") != 0) { // TODO: verify string is valid IP address
        destination_ip = convert_const_to_non_const(text); // set destination IP to new ip
        const gchar *prepend_text = "Chatting to: ";
        gchar *new_text = prepend_string(text, prepend_text);
        gtk_label_set_text(label_ip, new_text);
        g_free(new_text);
    }
}

void on_receive_message(const gchar *name, const gchar *message){
    push_message_to_queue(name, message);
}

void activate(GtkApplication *app, gpointer user_data) {
    set_gui_update_callback(on_receive_message);
    setupSocket();
    init_tls_handler();

    message_queue = g_async_queue_new();

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
    gtk_editable_set_text(GTK_EDITABLE(entry_ip), destination_ip);

    // set ip button
    button_ip = gtk_button_new_with_label("Set Destination IP");
    gtk_box_append(GTK_BOX(box_ip), button_ip);

    // ip info label
    gchar *new_text = append_string("Chatting to: ", destination_ip);
    label_ip = gtk_label_new(new_text);
    g_free(new_text);
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

    MessageData *message_data = g_malloc(sizeof(MessageData));
    message_data->name = "You:  ";
    message_data->text_view = GTK_TEXT_VIEW(text_view);

    // send message callback 
    g_signal_connect(entry_message, "activate", G_CALLBACK(on_display_message), message_data);

    // Present the window
    gtk_window_present(GTK_WINDOW(window));
}
