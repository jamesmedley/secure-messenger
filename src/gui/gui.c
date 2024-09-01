#include <gtk/gtk.h>
#include "gui.h"
#include "gui_utils.h"
#include "../networking/networking.h"
#include "../networking/tls_handler.h"
#include "../encryption/aes1.h"

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

SessionData *session;

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

        int plaintext_len = strlen(text);
        unsigned char *ciphertext = (unsigned char *)malloc(plaintext_len + EVP_MAX_BLOCK_LENGTH);
        if (ciphertext == NULL) {
            perror("Failed to allocate memory for ciphertext");
            return;
        }
        // Encrypt the plaintext
        int ciphertext_len = encrypt_aes(session->symmetric_key, session->iv, (unsigned char *)text, plaintext_len, ciphertext);
        if (ciphertext_len == -1) {
            fprintf(stderr, "Encryption failed\n");
            free(ciphertext);
            return;
        }

        // Send the encrypted message
        send_message((const char *)ciphertext, ciphertext_len, session->src_ip_address);
        free(ciphertext);
        push_message_to_queue(name, text);
        gtk_editable_set_text(GTK_EDITABLE(entry), ""); // Clear the entry widget
    }
}

void on_set_ip(GtkWidget *button_ip, gpointer user_data){
    IpData *data = (IpData *)user_data;
    GtkEntry *entry_ip = data->entry_ip;
    GtkLabel *label_ip = data->label_ip;

    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry_ip));
    send_client_hello(convert_const_to_non_const(text));

    if (g_strcmp0(text, "") != 0) { // TODO: verify string is valid IP address
        const gchar *prepend_text = "Chatting to: ";
        gchar *new_text = prepend_string(text, prepend_text);
        gtk_label_set_text(label_ip, new_text);
        g_free(new_text);
    } 
}

void on_receive_message(const gchar *name, const gchar *message, int message_len){
    unsigned char *decryptedtext = (unsigned char *)malloc(message_len + 1); 
       
    int decryptedtext_len = decrypt_aes(session->symmetric_key, session->iv, message, message_len, decryptedtext);
    if (decryptedtext_len < 0) {
        fprintf(stderr, "Decryption failed\n");
        return;
    }
    decryptedtext[decryptedtext_len] = '\0'; // Null-terminate the decrypted string
    push_message_to_queue(name, (const gchar *)decryptedtext);
    free(decryptedtext);
}

void on_choose(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    GError *err = NULL;
    gchar **params = (gchar **)user_data;
    gchar *src_ip_address = params[0];
    int button = gtk_alert_dialog_choose_finish(dialog, res, &err);

    if(err) {
        g_print("An error occured!\n");
        g_print("Error Message: %s\n", err->message);
        g_error_free(err);
        err = NULL;
        return;
    }
    switch(button){
        case 0:
            break;
        case 1:
            // generate session_id
            int session_id = 1; // e.g.
            send_server_hello(session_id, src_ip_address);
            const gchar *prepend_text = "Chatting to: "; // set new ip in chatting to label
            gchar *new_text = prepend_string(src_ip_address, prepend_text);
            gtk_label_set_text(GTK_LABEL(label_ip), new_text);
            gtk_editable_set_text(GTK_EDITABLE(entry_ip), src_ip_address);
            g_free(new_text);
            break;
        default:
            g_assert_not_reached();
            break;
    }}

int alert_dialog_thread_safe(gpointer user_data) {
    gchar **params = (gchar **)user_data;
    gchar *src_ip_address = params[0];
    // Create and display alert dialog
    if (window) {
        GtkAlertDialog *dialog = gtk_alert_dialog_new("Connection Request");
        const char* buttons[] = {"Decline", "Accept", NULL};
        gtk_alert_dialog_set_detail(dialog, g_strdup_printf("%s wants to chat...", src_ip_address));
        gtk_alert_dialog_set_buttons(dialog, buttons);
        gtk_alert_dialog_set_cancel_button(dialog, 0);
        gtk_alert_dialog_set_default_button(dialog, 1);
        gtk_alert_dialog_choose(dialog, GTK_WINDOW(window), NULL, on_choose, user_data);
    } else {
        g_print("Error: Main window is not initialized.\n");
    }
    g_free(params);

    return FALSE;
}

void on_new_session(gchar *symmetric_key, gchar *iv, int key_length, gchar *src_ip_address){
    // Save session data
    free(session->iv);
    free(session->symmetric_key);
    free(session);
    session = g_malloc(sizeof(SessionData));
    session->symmetric_key = g_strdup(symmetric_key);
    session->iv = g_strdup(iv);
    session->key_length = key_length; 
    session->src_ip_address = g_strdup(src_ip_address);
    // print_hex("Generated Symmetric Key", session->symmetric_key, 32);
    // print_hex("Generated IV", session->iv, 16);
    // printf("src_ip_address: %s\n", session->src_ip_address); 
}

void on_connection_req(gchar *src_ip_address){
    gchar **params = g_new(gchar*, 1);
    params[0] = g_strdup(src_ip_address);
    g_idle_add(alert_dialog_thread_safe, params);
}


void activate(GtkApplication *app, gpointer user_data) {
    set_gui_update_callback(on_receive_message);
    set_new_session_callback(on_new_session);
    set_connection_req_callback(on_connection_req);
    setupSocket();
    init_tls_handler();

    // initialise localhost session 
    initialize_openssl();
    unsigned char symmetric_key[32];
    unsigned char iv[16];
    if (generate_keys(symmetric_key, iv) != 0) {
        fprintf(stderr, "Key generation failed\n");
        cleanup_openssl();
        return;
    }
    cleanup_openssl();

    session = g_malloc(sizeof(SessionData));
    session->symmetric_key = g_strdup(symmetric_key);
    session->iv = g_strdup(iv);
    session->key_length = 32;
    session->src_ip_address = "127.0.0.1"; // localhost by default

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
    gtk_editable_set_text(GTK_EDITABLE(entry_ip), session->src_ip_address);

    // set ip button
    button_ip = gtk_button_new_with_label("Set Destination IP");
    gtk_box_append(GTK_BOX(box_ip), button_ip);

    // ip info label
    gchar *new_text = append_string("Chatting to: ", session->src_ip_address);
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