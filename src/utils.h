#ifndef __UTILS_H
#define __UTILS_H

#include <gtk/gtk.h>

/**
 * @brief Print error with custom formating
 * 
 * @param when 
 * @param code 
 */
void xapp_error(const char *when, int code);

/**
 * @brief Set the icon to XClickers icon for a window
 * 
 * @param window 
 */
void set_window_icon(GtkWindow *window);

/**
 * @brief Set the text of a GtkEntry widget if the provided text is not NULL.
 *
 * This function sets the text of the given GtkEntry widget if the provided
 * text is not NULL. If the text is NULL, no action is taken.
 *
 * @param entry A pointer to the GtkEntry widget.
 * @param text The text to set in the GtkEntry widget (can be NULL).
 */
void gtk_entry_set_text_if_not_null(GtkEntry *entry, const gchar *text);

#endif
