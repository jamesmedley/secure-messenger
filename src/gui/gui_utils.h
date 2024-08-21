void get_current_time_string(char *buffer, size_t buffer_size);

gchar* prepend_string(const gchar* original, const gchar* to_prepend);

gchar* append_string(const gchar* original, const gchar* to_append);

gchar* convert_const_to_non_const(const char* original);

void insert_colored_text(GtkTextBuffer *buffer, const gchar *text, const gchar *color);

void write_message(const gchar *name, const gchar *text, GtkTextBuffer *buffer, GtkTextView *text_view);
