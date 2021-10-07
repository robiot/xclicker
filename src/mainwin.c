#include <gtk/gtk.h>
#include <stdbool.h>

#include "xclicker-app.h"
#include "mainwin.h"
#include "x11.h"

struct _MainAppWindow
{
  GtkApplicationWindow parent;

  // Inputs
  GtkWidget* hours_entry;
  GtkWidget* minutes_entry;
  GtkWidget* seconds_entry;
  GtkWidget* millisecs_entry;

  // Bottom buttons
  GtkWidget *start_button;
  GtkWidget *stop_button;
  GtkWidget *settings_button;
} mainappwindow;
G_DEFINE_TYPE(MainAppWindow, main_app_window, GTK_TYPE_APPLICATION_WINDOW);

bool isClicking = false;

struct _click_opts {
  int sleep;
  int button;
} click_opts;

void *click_handler()
{
  Display *display = get_display();
  while (isClicking)
  {
    click(display, Button1);
    usleep(100 * 1000); // Milliseconds
  }
  XCloseDisplay(display);
}

int get_text_to_int(GtkWidget *entry) {
  return atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
}

// Handlers
// gint length, gint *position, gpointer user_data
void insert_handler(GtkEditable *editable, const gchar *text)
{
  if (!g_unichar_isdigit(g_utf8_get_char(text)))
  {
    g_signal_stop_emission_by_name(editable, "insert_text");
  }
}

void start_clicked(GtkButton *button)
{
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.stop_button), TRUE);
  isClicking = true;

  int sleep = get_text_to_int(mainappwindow.hours_entry) * 3600000
    + get_text_to_int(mainappwindow.minutes_entry) * 60000
    + get_text_to_int(mainappwindow.seconds_entry) * 1000
    + get_text_to_int(mainappwindow.millisecs_entry);

  click_opts.button = Button1;
  click_opts.sleep = sleep;
  g_thread_new("click_handler", click_handler, NULL);
}

void stop_clicked(GtkButton *button)
{
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.start_button), TRUE);
  isClicking = false;
}

static void main_app_window_init(MainAppWindow *win)
{
  gtk_widget_init_template(GTK_WIDGET(win));


  // Inputs
  mainappwindow.hours_entry = win->hours_entry;
  mainappwindow.minutes_entry = win->minutes_entry;
  mainappwindow.seconds_entry = win->seconds_entry;
  mainappwindow.millisecs_entry = win->millisecs_entry;
  // Buttons
  mainappwindow.start_button = win->start_button;
  mainappwindow.stop_button = win->stop_button;
  mainappwindow.settings_button = win->settings_button;
}

static void main_app_window_class_init(MainAppWindowClass *class)
{
  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/gtk/xclicker/ui/xclicker-window.ui");
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), insert_handler);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), start_clicked);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), stop_clicked);

  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, start_button);
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, stop_button);
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, settings_button);

  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hours_entry);
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, minutes_entry);
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, seconds_entry);
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, millisecs_entry);
}

MainAppWindow *main_app_window_new(XClickerApp *app)
{
  return g_object_new(MAIN_APP_WINDOW_TYPE, "application", app, NULL);
}

void main_app_window_open(MainAppWindow *win, GFile *file)
{
}