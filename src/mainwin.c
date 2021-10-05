#include <gtk/gtk.h>

#include "xclicker-app.h"
#include "mainwin.h"

struct _MainAppWindow
{
  GtkApplicationWindow parent;
};

G_DEFINE_TYPE(MainAppWindow, main_app_window, GTK_TYPE_APPLICATION_WINDOW);

// Handlers
void insert_handler(GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer user_data)
{
  if (!g_unichar_isdigit(g_utf8_get_char(text)))
  {
    g_signal_stop_emission_by_name(editable, "insert_text");
  }
}

static void main_app_window_init(MainAppWindow *win)
{
  gtk_widget_init_template(GTK_WIDGET(win));
}

static void main_app_window_class_init(MainAppWindowClass *class)
{
  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/gtk/xclicker/ui/xclicker-window.ui");
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), insert_handler);
}

MainAppWindow *main_app_window_new(XClickerApp *app)
{
  return g_object_new(MAIN_APP_WINDOW_TYPE, "application", app, NULL);
}

void main_app_window_open(MainAppWindow *win, GFile *file)
{
}