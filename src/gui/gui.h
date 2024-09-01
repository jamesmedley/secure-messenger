void activate(GtkApplication *app, gpointer user_data);

void on_receive_message(const gchar *name, const gchar *message, int message_len);
void on_new_session(gchar *encrypted_symmetric_key, gchar *iv, int key_length, gchar *src_ip_address);

typedef struct {
    GtkEntry *entry_ip;
    GtkLabel *label_ip;
} IpData;

typedef struct {
    gchar *name;
    GtkTextView *text_view;
} MessageData;

typedef struct {
    gchar *name;
    gchar *message;
    GtkTextView *text_view;
} MessageQueueData;

typedef struct {
    gchar *symmetric_key;
    gchar *iv;
    int key_length;
    gchar *src_ip_address;
} SessionData;