void activate(GtkApplication *app, gpointer user_data);

void on_receive_message(const gchar *name, const gchar *message);

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