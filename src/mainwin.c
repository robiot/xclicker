#include <gtk/gtk.h>
#include <sys/time.h>
#include "xclicker-app.h"
#include "mainwin.h"
#include "x11api.h"
#include "settings.h"
#include "macros.h"

struct _MainAppWindow
{
	GtkApplicationWindow parent;
	GtkWidget *pwin;

	// Entries
	GtkWidget *hours_entry;
	GtkWidget *minutes_entry;
	GtkWidget *seconds_entry;
	GtkWidget *millisecs_entry;
	GtkWidget *repeat_entry;
	GtkWidget *mouse_entry;
	GtkWidget *x_entry;
	GtkWidget *y_entry;

	// Checkboxes
	GtkWidget *repeat_only_check;
	GtkWidget *custom_location_check;

	// Bottom buttons
	GtkWidget *start_button;
	GtkWidget *stop_button;
	GtkWidget *settings_button;
	GtkWidget *get_button;
} mainappwindow;
G_DEFINE_TYPE(MainAppWindow, main_app_window, GTK_TYPE_APPLICATION_WINDOW);

gboolean isClicking = FALSE;
gboolean isChoosingLocation = FALSE;
// gboolean beforeWindowTopmost = FALSE; // For later.

struct click_opts
{
	int sleep;
	int button;

	gboolean repeat;
	int repeat_times;

	gboolean custom_location;
	int custom_x, custom_y;
};

void toggle_buttons()
{
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.start_button), !isClicking);
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.stop_button), isClicking);
}

void click_handler(gpointer *data)
{
	struct click_opts *args = data;
	Display *display = get_display();
	int count = 0;
	while (isClicking)
	{
		if (args->custom_location)
			move_to(display, args->custom_x, args->custom_y);
		
		if (click(display, args->button, is_using_xevent()) == FALSE)
			g_printerr("Error when sending click");

		usleep(args->sleep * 1000); // Milliseconds
		if (args->repeat)
		{
			if (count >= args->repeat_times)
				isClicking = FALSE;
			else
				count++;
		}
	}
	g_free(data);
	XCloseDisplay(display);
	g_idle_add(toggle_buttons, NULL);
}

struct set_coord_args
{
	const char *coordx;
	const char *coordy;
};

void set_coords(gpointer *data)
{
	struct set_coord_args *args = data;
	if (GTK_IS_ENTRY(mainappwindow.x_entry))
		gtk_entry_set_text(mainappwindow.x_entry, args->coordx);
	if (GTK_IS_ENTRY(mainappwindow.y_entry))
		gtk_entry_set_text(mainappwindow.y_entry, args->coordy);
	g_free(args);
}

void get_cursor_pos_click_handler()
{
	Display *display = get_display();
	mask_config(display, MASK_CONFIG_MOUSE);

	while (isChoosingLocation)
	{
		if (get_button_state(display) == 1)
			isChoosingLocation = FALSE;
	}
	XCloseDisplay(display);
}

// Toggle window settings if choosing location or finished.
void toggle_window_from_set()
{
	switch (isChoosingLocation)
	{
	case TRUE:
		gtk_window_set_keep_above(GTK_WINDOW(mainappwindow.pwin), TRUE);
		gtk_button_set_label(GTK_BUTTON(mainappwindow.get_button), "Click");
		gtk_widget_set_sensitive(mainappwindow.get_button, FALSE);
		break;
	case FALSE:
		gtk_window_set_keep_above(GTK_WINDOW(mainappwindow.pwin), FALSE);
		gtk_button_set_label(GTK_BUTTON(mainappwindow.get_button), "Get");
		gtk_widget_set_sensitive(mainappwindow.get_button, TRUE);
		break;
	}
}

void get_cursor_pos_handler()
{
	Display *display = get_display();
	while (isChoosingLocation)
	{
		int i_cur_x, i_cur_y;
		get_mouse_coords(display, &i_cur_x, &i_cur_y);
		char *cur_x = (char *)malloc(1 + sizeof(i_cur_x));
		char *cur_y = (char *)malloc(1 + sizeof(i_cur_y));
		sprintf(cur_x, "%d", i_cur_x);
		sprintf(cur_y, "%d", i_cur_y);

		struct set_coord_args *data = g_malloc0(sizeof(struct set_coord_args));
		data->coordx = cur_x;
		data->coordy = cur_y;
		g_idle_add(set_coords, data);

		usleep(50000);
		free(cur_x);
		free(cur_y);
	}
	XCloseDisplay(display);
	g_idle_add(toggle_window_from_set, NULL);
}

int get_text_to_int(GtkWidget *entry)
{
	return atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
}

// Handlers
void repeat_only_check_toggle(GtkToggleButton *check)
{
	gtk_widget_set_sensitive(mainappwindow.repeat_entry, gtk_toggle_button_get_active(check));
}

void custom_location_check_toggle(GtkToggleButton *check)
{
	gboolean active = gtk_toggle_button_get_active(check);
	gtk_widget_set_sensitive(mainappwindow.x_entry, active);
	gtk_widget_set_sensitive(mainappwindow.y_entry, active);
	gtk_widget_set_sensitive(mainappwindow.get_button, active);
}

// gint length, gint *position, gpointer user_data
void insert_handler(GtkEditable *editable, const gchar *text)
{
	if (!g_unichar_isdigit(g_utf8_get_char(text)))
	{
		g_signal_stop_emission_by_name(editable, "insert_text");
	}
}

void open_safe_mode_dialog()
{
	GtkDialog *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Warning");
	gtk_message_dialog_format_secondary_text(dialog, "Intervals under 10 milliseconds is restricted because of safe mode.");
	gtk_dialog_run(dialog);
	gtk_widget_destroy(dialog);
}

void start_clicked()
{
	// This is ran inside another thread when used
	int sleep = get_text_to_int(mainappwindow.hours_entry) * 3600000 + get_text_to_int(mainappwindow.minutes_entry) * 60000 + get_text_to_int(mainappwindow.seconds_entry) * 1000 + get_text_to_int(mainappwindow.millisecs_entry);

	if (sleep < 10 && is_safemode())
	{
		g_idle_add(open_safe_mode_dialog, NULL);
		return;
	}

	isClicking = TRUE;
	toggle_buttons();

	// click_opts.button = Button1;
	struct click_opts *data = g_malloc0(sizeof(struct click_opts));

	data->sleep = sleep;
	const gchar *selectedtext = gtk_entry_get_text(GTK_ENTRY(mainappwindow.mouse_entry));
	if (strcmp(selectedtext, "Right") == 0)
		data->button = Button3;
	else if (strcmp(selectedtext, "Middle") == 0)
		data->button = Button2;
	else
		data->button = Button1;

	if ((data->repeat = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.repeat_only_check))))
		data->repeat_times = get_text_to_int(mainappwindow.repeat_entry);

	if ((data->custom_location = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.custom_location_check))))
	{
		data->custom_x = get_text_to_int(mainappwindow.x_entry);
		data->custom_y = get_text_to_int(mainappwindow.y_entry);
	}
	g_thread_new("click_handler", click_handler, data);
}

void stop_clicked()
{
	isClicking = FALSE;
	toggle_buttons();
}

void settings_clicked()
{
	settings_dialog_new();
}

void get_button_clicked()
{
	isChoosingLocation = TRUE;
	toggle_window_from_set();
	g_thread_new("get_cursor_pos_click_handler", get_cursor_pos_click_handler, NULL);
	g_thread_new("get_cursor_pos_handler", get_cursor_pos_handler, NULL);
}

void toggle_clicking()
{
	if (isClicking)
		stop_clicked();
	else
		start_clicked();
}

// Has to be here since it calls start_clicked
void get_start_stop_key_handler()
{
	Display *display = get_display();
	mask_config(display, MASK_CONFIG_KEYBOARD);

	// 50 = shift
	struct timeval start, stop;
	double secs = 0;
	int before = 0;
	while (1)
	{
		int state = get_button_state(display);
		if (isChoosingHotkey == TRUE)
			continue;
		// g_print("%s, %d\n", keycode_to_string(display, state), state);

		// F8, 74
		if (state == button1 || state == button2)
		{
			// Two buttons
			if (button1 != -1)
			{
				if ((before == button1 || before == button2) && before != state)
				{
					gettimeofday(&stop, NULL);
					secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
					if (secs < 0.1)
						toggle_clicking();
					before = 0;
				}
				else
				{
					gettimeofday(&start, NULL);
					before = state;
				}
			}
			// One button
			else
				toggle_clicking();
			usleep(5000);
		}
	}
	XCloseDisplay(display);
}

void set_start_stop_button_keybind_text()
{
	Display *display = get_display();
	const char *start_text_1 = "Start";
	const char *stop_text_1 = "Stop";

	// Button2 should always be defined
	const char *button_2_key = keycode_to_string(display, button2);

	char *start_text;
	char *stop_text;

	// If 2 keys
	if (button1 != -1)
	{
		const char *button_1_key = keycode_to_string(display, button1);
		start_text = malloc(strlen(start_text_1) + strlen(button_1_key) + strlen(button_2_key) + 4);
		stop_text = malloc(strlen(stop_text_1) + strlen(button_1_key) + strlen(button_2_key) + 4);
		sprintf(start_text, "%s (%s+%s)", start_text_1, button_1_key, button_2_key);
		sprintf(stop_text, "%s (%s+%s)", stop_text_1, button_1_key, button_2_key);
	}
	// Only one key
	else
	{
		start_text = malloc(strlen(start_text_1) + strlen(button_2_key) + 3);
		stop_text = malloc(strlen(stop_text_1) + strlen(button_2_key) + 3);
		sprintf(start_text, "%s (%s)", start_text_1, button_2_key);
		sprintf(stop_text, "%s (%s)", stop_text_1, button_2_key);
	}

	gtk_button_set_label(GTK_BUTTON(mainappwindow.start_button), start_text);
	gtk_button_set_label(GTK_BUTTON(mainappwindow.stop_button), stop_text);
	/*
	With this free you can either get
	"corrupted size vs. prev_size" or "free(): invalid next size (fast)""
	if you use Shift + Numpad as keys.
	This is why numpad is disabled.
	*/
	free(start_text);
	free(stop_text);
	XCloseDisplay(display);
}

static void main_app_window_init(MainAppWindow *win)
{
	gtk_widget_init_template(GTK_WIDGET(win));
	config_init();
	load_start_stop_keybinds();

	mainappwindow.pwin = gtk_widget_get_toplevel(win);

	// Entries
	mainappwindow.hours_entry = win->hours_entry;
	mainappwindow.minutes_entry = win->minutes_entry;
	mainappwindow.seconds_entry = win->seconds_entry;
	mainappwindow.millisecs_entry = win->millisecs_entry;
	mainappwindow.repeat_entry = win->repeat_entry;
	mainappwindow.mouse_entry = win->mouse_entry;
	mainappwindow.x_entry = win->x_entry;
	mainappwindow.y_entry = win->y_entry;

	// Checkboxes
	mainappwindow.repeat_only_check = win->repeat_only_check;
	mainappwindow.custom_location_check = win->custom_location_check;

	// Buttons
	mainappwindow.start_button = win->start_button;
	mainappwindow.stop_button = win->stop_button;
	mainappwindow.settings_button = win->settings_button;
	mainappwindow.get_button = win->get_button;

	set_start_stop_button_keybind_text();
	g_thread_new("get_start_stop_key_handler", get_start_stop_key_handler, NULL);
}

static void main_app_window_class_init(MainAppWindowClass *class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/gtk/xclicker/ui/xclicker-window.ui");
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), insert_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), start_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), stop_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), repeat_only_check_toggle);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), settings_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), custom_location_check_toggle);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), get_button_clicked);

	// Entries
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hours_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, minutes_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, seconds_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, millisecs_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, repeat_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, mouse_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, x_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, y_entry);

	// Checkboxes
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, repeat_only_check);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, custom_location_check);

	// Buttons
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, start_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, stop_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, settings_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, get_button);
}

MainAppWindow *main_app_window_new(XClickerApp *app)
{
	return g_object_new(MAIN_APP_WINDOW_TYPE, "application", app, NULL);
}

void main_app_window_open(MainAppWindow *UNUSED(win), GFile *UNUSED(file))
{
}