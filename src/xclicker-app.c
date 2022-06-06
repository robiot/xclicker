#include <gtk/gtk.h>

#include "xclicker-app.h"
#include "mainwin.h"

struct _XClickerApp
{
	GtkApplication parent;
};

G_DEFINE_TYPE(XClickerApp, xclicker_app, GTK_TYPE_APPLICATION);

static void xclicker_app_init(XClickerApp* /*app*/)
{
}

/**
 * Opens up main window.
 */
static void xclicker_app_activate(GApplication *app)
{
	MainAppWindow *win = main_app_window_new(XCLICKER_APP(app));
	gtk_window_present(GTK_WINDOW(win));
}


static void xclicker_app_class_init(XClickerAppClass *class)
{
	G_APPLICATION_CLASS(class)->activate = xclicker_app_activate;
}

XClickerApp *xclicker_app_new()
{
	return g_object_new(XCLICKER_APP_TYPE, NULL);
}
