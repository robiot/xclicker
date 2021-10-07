#include <gtk/gtk.h>
#include <stdbool.h>

#include "xclicker-app.h"
#include "mainwin.h"
#include "x11api.h"

struct _MainAppWindow
{
	GtkApplicationWindow parent;

	// Entries
	GtkWidget *hours_entry;
	GtkWidget *minutes_entry;
	GtkWidget *seconds_entry;
	GtkWidget *millisecs_entry;
	GtkWidget *repeat_entry;

	// Checkboxes
	GtkWidget *repeat_only_check;

	// Bottom buttons
	GtkWidget *start_button;
	GtkWidget *stop_button;
	GtkWidget *settings_button;
} mainappwindow;
G_DEFINE_TYPE(MainAppWindow, main_app_window, GTK_TYPE_APPLICATION_WINDOW);

bool isClicking = false;
struct _click_opts
{
	int sleep;
	int button;

	gboolean repeat;
	int repeat_times;
} click_opts;

void toggle_buttons() {
	bool activated = isClicking;
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.start_button), !activated);
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.stop_button), activated);
}

void *click_handler()
{
	Display *display = get_display();
	int count = 0;
	while (isClicking)
	{
		click(display, click_opts.button);
		usleep(click_opts.sleep * 1000); // Milliseconds
		if (click_opts.repeat)
		{
			if (count >= click_opts.repeat_times)
			{
				isClicking = false;
			} else {
				count++;
			}
		}
	}
	// Free?
	XCloseDisplay(display);
	g_idle_add(toggle_buttons, false);
}

int get_text_to_int(GtkWidget *entry)
{
	return atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
}

// Handlers
void repeat_only_check_toggle(GtkWidget *check)
{
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.repeat_entry), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check)));
}

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
	isClicking = true;
	toggle_buttons();

	int sleep = get_text_to_int(mainappwindow.hours_entry) * 3600000 + get_text_to_int(mainappwindow.minutes_entry) * 60000 + get_text_to_int(mainappwindow.seconds_entry) * 1000 + get_text_to_int(mainappwindow.millisecs_entry);

	click_opts.button = Button1;
	click_opts.sleep = sleep;
	click_opts.repeat = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.repeat_only_check));
	if (click_opts.repeat)
		click_opts.repeat_times = get_text_to_int(mainappwindow.repeat_entry);

	g_thread_new("click_handler", click_handler, NULL);
}

void stop_clicked(GtkButton *button)
{
	isClicking = false;
	toggle_buttons();
}

static void main_app_window_init(MainAppWindow *win)
{
	gtk_widget_init_template(GTK_WIDGET(win));

	// Entries
	mainappwindow.hours_entry = win->hours_entry;
	mainappwindow.minutes_entry = win->minutes_entry;
	mainappwindow.seconds_entry = win->seconds_entry;
	mainappwindow.millisecs_entry = win->millisecs_entry;
	mainappwindow.repeat_entry = win->repeat_entry;

	// Checkboxes
	mainappwindow.repeat_only_check = win->repeat_only_check;

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
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), repeat_only_check_toggle);

	// Entries
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hours_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, minutes_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, seconds_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, millisecs_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, repeat_entry);

	// Checkboxes
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, repeat_only_check);

	// Buttons
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, start_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, stop_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, settings_button);
}

MainAppWindow *main_app_window_new(XClickerApp *app)
{
	return g_object_new(MAIN_APP_WINDOW_TYPE, "application", app, NULL);
}

void main_app_window_open(MainAppWindow *win, GFile *file)
{
}