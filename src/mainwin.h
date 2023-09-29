#ifndef __MAINWIN_H
#define __MAINWIN_H

#include <gtk/gtk.h>
#include "xclicker-app.h"

#define MAIN_APP_WINDOW_TYPE (main_app_window_get_type())
G_DECLARE_FINAL_TYPE(MainAppWindow, main_app_window, XCLICKER, APP_WINDOW, GtkApplicationWindow)


/**
 * Set corresponding fields to their imported config value
*/
void mainappwindow_import_config();

/**
 * Set the Start & Stop button text from configured hotkey
 */
void set_start_stop_button_hotkey_text();

MainAppWindow *main_app_window_new(XClickerApp *app);
#endif
