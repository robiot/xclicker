#ifndef __MAINWIN_H
#define __MAINWIN_H

#include <gtk/gtk.h>
#include "xclicker-app.h"


#define MAIN_APP_WINDOW_TYPE (main_app_window_get_type ())
G_DECLARE_FINAL_TYPE (MainAppWindow, main_app_window, XCLICKER, APP_WINDOW, GtkApplicationWindow)


MainAppWindow *main_app_window_new(XClickerApp *app);
void main_app_window_open(MainAppWindow *win, GFile *file);


#endif
