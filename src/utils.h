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

#endif
