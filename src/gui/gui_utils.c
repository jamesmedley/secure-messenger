#include <gtk/gtk.h>
#include "../networking/networking.h"

// Function to get the current time as a string in HH:MM format
void get_current_time_string(char *buffer, size_t buffer_size) {
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, buffer_size, "%H:%M", time_info);
}

// Function to prepend a string to an existing string
gchar* prepend_string(const gchar* original, const gchar* to_prepend) {
    gsize prepend_text_len = strlen(to_prepend);
    gsize existing_text_len = strlen(original);
    gsize new_text_len = prepend_text_len + existing_text_len;

    gchar *new_text = g_malloc(new_text_len + 1);  
    if (new_text == NULL) {
        g_error("Memory allocation failed\n");
        return NULL;
    }

    strcpy(new_text, to_prepend);
    strcat(new_text, original);

    return new_text;
}

// Function to append a string to an existing string
gchar* append_string(const gchar* original, const gchar* to_append) {
    gsize append_text_len = strlen(to_append);
    gsize existing_text_len = strlen(original);
    gsize new_text_len = append_text_len + existing_text_len;

    gchar *new_text = g_malloc(new_text_len + 1);
    if (new_text == NULL) {
        g_error("Memory allocation failed\n");
        return NULL;
    }

    strcpy(new_text, original);
    strcat(new_text, to_append);

    return new_text;
}

// Function to convert a const gchar* to gchar*
gchar* convert_const_to_non_const(const gchar* original) {
    gchar* new_string = g_malloc(strlen(original) + 1);
    if (new_string == NULL) {
        g_error("Memory allocation failed\n");
        return NULL;
    }

    strcpy(new_string, original);

    return new_string;
}

void write_message(const gchar *name, const gchar *text, GtkTextBuffer *buffer, GtkTextView *text_view) {
    gchar time_string[6]; // Buffer to hold the time string (HH:MM \0)
    get_current_time_string(time_string, sizeof(time_string));

    // Construct message parts
    gchar *time_string_tabbed = append_string(time_string, "  ");
    gchar *name_string_tabbed = append_string(name, "  ");

    if (time_string_tabbed == NULL || name_string_tabbed == NULL) {
        g_error("Memory allocation failed\n");
        return;
    }

    // Get a valid iterator at the end of the buffer
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    // Get tag table and lookup or create needed tags
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
    
    GtkTextTag *grey_tag = gtk_text_tag_table_lookup(tag_table, "grey");
    if (grey_tag == NULL) {
        grey_tag = gtk_text_tag_new("grey");
        g_object_set(grey_tag, "foreground", "grey", NULL);
        gtk_text_tag_table_add(tag_table, grey_tag);
    }

    GtkTextTag *red_tag = gtk_text_tag_table_lookup(tag_table, "red");
    if (red_tag == NULL) {
        red_tag = gtk_text_tag_new("red");
        g_object_set(red_tag, "foreground", "red", NULL);
        gtk_text_tag_table_add(tag_table, red_tag);
    }

    GtkTextTag *black_tag = gtk_text_tag_table_lookup(tag_table, "black");
    if (black_tag == NULL) {
        black_tag = gtk_text_tag_new("black");
        g_object_set(black_tag, "foreground", "black", NULL);
        gtk_text_tag_table_add(tag_table, black_tag);
    }

    // Insert the entire message in one call
    gtk_text_buffer_insert_with_tags(buffer, &end_iter, time_string_tabbed, -1, grey_tag, NULL);
    gtk_text_buffer_insert_with_tags(buffer, &end_iter, name_string_tabbed, -1, red_tag, NULL);
    gtk_text_buffer_insert_with_tags(buffer, &end_iter, text, -1, black_tag, NULL);
    gtk_text_buffer_insert_with_tags(buffer, &end_iter, "\n", -1, black_tag, NULL);

    // Scroll to the end of the buffer
    gtk_text_view_scroll_to_iter(text_view, &end_iter, 0.0, FALSE, 0.0, 0.0);

    g_free(name_string_tabbed);
    g_free(time_string_tabbed);
}

