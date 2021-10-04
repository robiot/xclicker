#ifndef __XCLICKERAPP_H
#define __XCLICKERAPP_H

#include <gtk/gtk.h>


#define XCLICKER_APP_TYPE (xclicker_app_get_type())
G_DECLARE_FINAL_TYPE(XClickerApp, xclicker_app, XCLICKER, APP, GtkApplication)

XClickerApp *xclicker_app_new(void);

#endif