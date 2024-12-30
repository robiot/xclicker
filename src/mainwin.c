#include <gtk/gtk.h>
#include <sys/time.h>
#include "xclicker-app.h"
#include "mainwin.h"
#include "x11api.h"
#include "settings.h"
#include "utils.h"
#include "config.h"

enum ClickTypes
{
	CLICK_TYPE_SINGLE,
	CLICK_TYPE_DOUBLE,
	CLICK_TYPE_HOLD,
};

enum HoldTimeTypes
{
	HOLDTIME_TYPE_CONSTANT,
	HOLDTIME_TYPE_RANDOM,
};

gboolean isClicking = FALSE;
gboolean isChoosingLocation = FALSE;

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
	GtkWidget *click_type_entry;
	GtkWidget *x_entry;
	GtkWidget *y_entry;
	GtkWidget *random_interval_entry;
	GtkWidget *hotkey_type_entry;
	GtkWidget *hold_time_entry;
	GtkWidget *holdtime_type_entry;

	// Checkboxes
	GtkWidget *repeat_only_check;
	GtkWidget *custom_location_check;
	GtkWidget *random_interval_check;
	GtkWidget *hold_time_check;

	// Buttons
	GtkWidget *start_button;
	GtkWidget *stop_button;
	GtkWidget *settings_button;
	GtkWidget *get_button;
} mainappwindow;
G_DEFINE_TYPE(MainAppWindow, main_app_window, GTK_TYPE_APPLICATION_WINDOW);

/**
 * Options/data given to click_handler thread
 */
struct click_opts
{
	int sleep;
	int button;
	int click_type;

	gboolean repeat;
	int repeat_times;

	gboolean custom_location;
	int custom_x, custom_y;

	gboolean random_interval;
	int random_interval_ms;

	gboolean hold_time;
	int hold_time_ms;
	int	holdtime_type;
};

/**
 * Generates a random integer between given values.
 * @param lower The lowest value it can generate
 * @param upper The highest value it can generate
 */
int random_between(int lower, int upper)
{
	return rand() % ((upper - lower + 1)) + lower;
}

/**
 * Convert given entries text to an integer.
 * @param entry The entry to get text from
 */
int get_text_to_int(GtkWidget *entry)
{
	return atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
}

/**
 * Toggle the start and stop buttons
 */
gboolean toggle_buttons()
{
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.start_button), !isClicking);
	gtk_widget_set_sensitive(GTK_WIDGET(mainappwindow.stop_button), isClicking);
	return FALSE;
}

/**
 * Handles the clicking, should be ran from another thread.
 * @param data Struct of type click_opts which contains the options
 * @see click_opts
 */
void click_handler(gpointer *data)
{
	struct click_opts *args = data;
	Display *display = get_display();
	int count = 0;
	gboolean is_holding = FALSE;

	gboolean using_xevent = config->use_xevent;

	int hold_type_ms = 0;
	if (args->hold_time == TRUE)
	{
		hold_type_ms=args->hold_time_ms* 1000 ;
	}

	while (isClicking)
	{
		if (args->custom_location)
			move_to(display, args->custom_x, args->custom_y);

		// reassigning a new hold time
		if (args->hold_time == TRUE && args->holdtime_type==HOLDTIME_TYPE_RANDOM)
		{
			hold_type_ms=random_between(0, args->hold_time_ms)* 1000 ;
		}

		switch (args->click_type)
		{
		case CLICK_TYPE_SINGLE:
			if (click(display, args->button, using_xevent, hold_type_ms) == FALSE)
				xapp_error("Sending click", -1);
			break;
		case CLICK_TYPE_DOUBLE:
			if (click(display, args->button, using_xevent, hold_type_ms) == FALSE)
				xapp_error("Sending click", -1);

			usleep(150000); // 150 milliseconds

			if (click(display, args->button, using_xevent, hold_type_ms) == FALSE)
				xapp_error("Sending click", -1);
			break;
		case CLICK_TYPE_HOLD:
			if (is_holding == FALSE) // Don't re-send mouse_down if already successfully sent
			{
				if (mouse_event(display, args->button, using_xevent, MOUSE_EVENT_PRESS))
					is_holding = TRUE;
				else
					xapp_error("Sending mouse down", -1);
			}
			break;
		}

		int sleep = args->sleep * 1000;
		if (args->random_interval)
		{
			int random_interval_ms = args->random_interval_ms * 1000;
			int lower = sleep - random_interval_ms;
			usleep(random_between((lower > 0) ? lower : 0, sleep + random_interval_ms));
		}
		else
			usleep(sleep);

		if (args->repeat)
		{
			if (count >= args->repeat_times)
				isClicking = FALSE;
			else
				count++;
		}
	}

	// If it was a mouse hold, then release the button
	if (args->click_type == CLICK_TYPE_HOLD)
	{
		if (mouse_event(display, args->button, config->use_xevent, MOUSE_EVENT_RELEASE) == FALSE)
			xapp_error("Sending mouse down", -1);
	}

	g_free(data);
	XCloseDisplay(display);
	g_idle_add(toggle_buttons, NULL);
}

/**
 *
 * Custom Location
 *
 */

/**
 * Data to pass to set_coords.
 */
struct set_coord_args
{
	const char *coordx;
	const char *coordy;
};

/**
 * Updates the x and y textboxes with the current cursor location.
 */
gboolean set_coords(gpointer *data)
{
	struct set_coord_args *args = data;
	if (GTK_IS_ENTRY(mainappwindow.x_entry))
		gtk_entry_set_text(mainappwindow.x_entry, args->coordx);
	if (GTK_IS_ENTRY(mainappwindow.y_entry))
		gtk_entry_set_text(mainappwindow.y_entry, args->coordy);

	g_free(args);

	return FALSE;
}

/**
 * Checks if left mouse button is pressed, then stop capturing
 * mouse location.
 */
void get_cursor_pos_click_handler()
{
	Display *display = get_display();
	mask_config(display, MASK_MOUSE_PRESS);

	while (isChoosingLocation)
	{
		KeyState keyState;
		get_next_key_state(display, &keyState);

		if (keyState.button == Button1) // 1 = Mouse1
			isChoosingLocation = FALSE;
	}
	XCloseDisplay(display);
}

/**
 * Toggle "Get button sensitive" and window topmost.
 */
gboolean toggle_get_active()
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
	return FALSE;
}

/**
 * Grabs the cursor location and updates the textboxes until
 * left mouse clicked.
 * @see get_cursor_pos_click_handler
 */
void get_cursor_pos_handler()
{
	Display *display = get_display();
	while (isChoosingLocation)
	{
		int i_cur_x, i_cur_y;
		get_cursor_coords(display, &i_cur_x, &i_cur_y);
		// before allocating, get how long the numbers would be as a string
		int x_len = snprintf(NULL, 0, "%d", i_cur_x);
		int y_len = snprintf(NULL, 0, "%d", i_cur_y);
		char *cur_x = (char *)malloc(1 + x_len);
		char *cur_y = (char *)malloc(1 + y_len);
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
	g_idle_add(toggle_get_active, NULL);
}

/**
 *
 * Handlers
 *
 */

void repeat_only_check_toggle(GtkToggleButton *check)
{
	gtk_widget_set_sensitive(mainappwindow.repeat_entry, gtk_toggle_button_get_active(check));

	g_key_file_set_boolean(config_gfile, PCK_REPEAT, gtk_toggle_button_get_active(check));
	save_and_populate_config();
}

void custom_location_check_toggle(GtkToggleButton *check)
{
	gboolean active = gtk_toggle_button_get_active(check);
	gtk_widget_set_sensitive(mainappwindow.x_entry, active);
	gtk_widget_set_sensitive(mainappwindow.y_entry, active);
	gtk_widget_set_sensitive(mainappwindow.get_button, active);

	g_key_file_set_boolean(config_gfile, PCK_CUSTOM_LOCATION, active);
	save_and_populate_config();
}

/**
 * Prevents other input than 0-9
 */
void insert_handler(GtkEditable *editable, const gchar *text)
{
	if (!g_unichar_isdigit(g_utf8_get_char(text)))
	{
		g_signal_stop_emission_by_name(editable, "insert_text");
	}
}

// Input change events
void input_changed_save_handler(GtkEditable *editable, struct _MainAppWindow *mainappwindow)
{
	GtkEntry *entry = GTK_ENTRY(editable);
	const gchar *text = gtk_entry_get_text(entry);

	if (editable == GTK_EDITABLE(mainappwindow->hours_entry))
	{
		g_key_file_set_string(config_gfile, PCK_HOURS, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->minutes_entry))
	{
		g_key_file_set_string(config_gfile, PCK_MINUTES, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->seconds_entry))
	{
		g_key_file_set_string(config_gfile, PCK_SECONDS, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->millisecs_entry))
	{
		g_key_file_set_string(config_gfile, PCK_MILLISECS, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->repeat_entry))
	{
		g_key_file_set_string(config_gfile, PCK_REPEAT_TIMES, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->random_interval_entry))
	{
		g_key_file_set_string(config_gfile, PCK_RANDOM_INTERVAL_MS, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->hold_time_entry))
	{
		g_key_file_set_string(config_gfile, PCK_HOLD_TIME_MS, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->x_entry))
	{
		g_key_file_set_string(config_gfile, PCK_CUSTOM_X, text);
	}
	else if (editable == GTK_EDITABLE(mainappwindow->y_entry))
	{
		g_key_file_set_string(config_gfile, PCK_CUSTOM_Y, text);
	}

	save_and_populate_config();
}

gboolean open_safe_mode_dialog()
{
	GtkDialog *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Warning");
	gtk_message_dialog_format_secondary_text(dialog, "Intervals under 100 milliseconds is restricted because of safe mode.");
	gtk_dialog_run(dialog);
	gtk_widget_destroy(dialog);
	return FALSE;
}

/**
 * Grab click options from ui and pass them to click_opts, then start click_handler.
 * @see click_handler
 */
void start_clicked()
{
	int sleep = get_text_to_int(mainappwindow.hours_entry) * 3600000 + get_text_to_int(mainappwindow.minutes_entry) * 60000 + get_text_to_int(mainappwindow.seconds_entry) * 1000 + get_text_to_int(mainappwindow.millisecs_entry);

	if (sleep < 100 && is_safemode())
	{
		g_idle_add(open_safe_mode_dialog, NULL);
		return;
	}

	isClicking = TRUE;
	toggle_buttons();

	struct click_opts *data = g_malloc0(sizeof(struct click_opts));

	data->sleep = sleep;
	const gchar *mousebutton_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.mouse_entry));
	if (strcmp(mousebutton_text, "Right") == 0)
		data->button = Button3;
	else if (strcmp(mousebutton_text, "Middle") == 0)
		data->button = Button2;
	else
		data->button = Button1;

	const gchar *click_type_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.click_type_entry));
	if (strcmp(click_type_text, "Single") == 0)
		data->click_type = CLICK_TYPE_SINGLE;
	else if (strcmp(click_type_text, "Double") == 0)
		data->click_type = CLICK_TYPE_DOUBLE;
	else if (strcmp(click_type_text, "Hold") == 0)
		data->click_type = CLICK_TYPE_HOLD;
	else
		xapp_error("Getting the click type", 1);

	if ((data->repeat = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.repeat_only_check))))
		data->repeat_times = get_text_to_int(mainappwindow.repeat_entry);

	if ((data->custom_location = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.custom_location_check))))
	{
		data->custom_x = get_text_to_int(mainappwindow.x_entry);
		data->custom_y = get_text_to_int(mainappwindow.y_entry);
	}

	if ((data->random_interval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.random_interval_check))))
		data->random_interval_ms = get_text_to_int(mainappwindow.random_interval_entry);

	const gchar *holdtime_type_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.holdtime_type_entry));
	if (strcmp(holdtime_type_text, "Constant") == 0)
		data->holdtime_type = HOLDTIME_TYPE_CONSTANT;
	else if (strcmp(holdtime_type_text, "Random") == 0)
		data->holdtime_type = HOLDTIME_TYPE_RANDOM;
	else
		xapp_error("Getting the hold time type", 1);

	if ((data->hold_time = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mainappwindow.hold_time_check))))
		data->hold_time_ms = get_text_to_int(mainappwindow.hold_time_entry);

	// If holding, ignore interval and repeat as it makes no sense.
	if (data->click_type == CLICK_TYPE_HOLD)
	{
		data->repeat = FALSE;
		data->random_interval = FALSE;
		data->hold_time = FALSE;
		data->sleep = 0;
	}

	g_thread_new("click_handler", click_handler, data);
}

void stop_clicked()
{
	isClicking = FALSE;
	toggle_buttons();
}

void mainappwindow_import_config()
{
	gtk_entry_set_text_if_not_null(mainappwindow.hours_entry, config->hours);
	gtk_entry_set_text_if_not_null(mainappwindow.minutes_entry, config->minutes);
	gtk_entry_set_text_if_not_null(mainappwindow.seconds_entry, config->seconds);
	gtk_entry_set_text_if_not_null(mainappwindow.millisecs_entry, config->millisecs);

	gtk_entry_set_text_if_not_null(mainappwindow.mouse_entry, config->mouse_button);
	gtk_entry_set_text_if_not_null(mainappwindow.click_type_entry, config->click_type);
	gtk_entry_set_text_if_not_null(mainappwindow.hotkey_type_entry, config->hotkey);
	gtk_toggle_button_set_active(mainappwindow.repeat_only_check, config->use_repeat);
	gtk_entry_set_text_if_not_null(mainappwindow.repeat_entry, config->repeat_times);

	gtk_toggle_button_set_active(mainappwindow.custom_location_check, config->use_custom_location);
	gtk_entry_set_text_if_not_null(mainappwindow.x_entry, config->custom_x);
	gtk_entry_set_text_if_not_null(mainappwindow.y_entry, config->custom_y);
	gtk_toggle_button_set_active(mainappwindow.random_interval_check, config->use_random_interval);
	gtk_entry_set_text_if_not_null(mainappwindow.random_interval_entry, config->random_interval_ms);
	gtk_toggle_button_set_active(mainappwindow.hold_time_check, config->use_hold_time);
	gtk_entry_set_text_if_not_null(mainappwindow.hold_time_entry, config->hold_time_ms);
	gtk_entry_set_text_if_not_null(mainappwindow.holdtime_type_entry, config->holdtime_type);
}

void settings_clicked()
{
	settings_dialog_new();
}

void get_button_clicked()
{
	isChoosingLocation = TRUE;
	toggle_get_active();
	g_thread_new("get_cursor_pos_click_handler", get_cursor_pos_click_handler, NULL);
	g_thread_new("get_cursor_pos_handler", get_cursor_pos_handler, NULL);
}

void click_type_entry_changed()
{
	const gchar *click_type_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.click_type_entry));
	gboolean active = TRUE;
	if (strcmp(click_type_text, "Hold") == 0)
	{
		active = FALSE;
	}

	g_key_file_set_string(config_gfile, PCK_CLICK_TYPE, click_type_text);
	g_key_file_save_to_file(config_gfile, configpath, NULL);

	gtk_widget_set_sensitive(mainappwindow.hours_entry, active);
	gtk_widget_set_sensitive(mainappwindow.minutes_entry, active);
	gtk_widget_set_sensitive(mainappwindow.seconds_entry, active);
	gtk_widget_set_sensitive(mainappwindow.millisecs_entry, active);
	gtk_widget_set_sensitive(mainappwindow.repeat_only_check, active);
	gtk_widget_set_sensitive(mainappwindow.repeat_entry, active);
	gtk_widget_set_sensitive(mainappwindow.random_interval_check, active);
	gtk_widget_set_sensitive(mainappwindow.random_interval_entry, active);
	gtk_widget_set_sensitive(mainappwindow.hold_time_check, active);
	gtk_widget_set_sensitive(mainappwindow.hold_time_entry, active);
}

void hotkey_type_entry_changed()
{
	const gchar *hotkey_type_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.hotkey_type_entry));

	g_key_file_set_string(config_gfile, PCK_HOTKEY, hotkey_type_text);
	g_key_file_save_to_file(config_gfile, configpath, NULL);
}

void mouse_button_entry_changed()
{
	const gchar *mouse_button_entry_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.mouse_entry));

	g_key_file_set_string(config_gfile, PCK_MOUSE_BUTTON, mouse_button_entry_text);
	g_key_file_save_to_file(config_gfile, configpath, NULL);
}

void toggle_clicking(int evtype)
{
	if (strcmp(gtk_entry_get_text(GTK_ENTRY(mainappwindow.hotkey_type_entry)), "Normal"))
	{
		if (evtype == KeyPress)
		{
			start_clicked();
		}
		else
		{
			stop_clicked();
		}
	}
	else if (evtype == KeyPress)
	{
		if (isClicking)
			stop_clicked();
		else
			start_clicked();
	}
}

/**
 * Gets if configured hotkey is pressed, if so
 * toggle clicking.
 */
void get_start_stop_key_handler()
{
	Display *display = get_display();
	mask_config(display, MASK_KEYBOARD_PRESS | MASK_KEYBOARD_RELEASE);

	gboolean isHolding1 = FALSE;
	gboolean isHolding2 = FALSE;

	while (1)
	{
		KeyState keyState;
		get_next_key_state(display, &keyState);
		if (isChoosingHotkey == TRUE)
			continue;

		if (keyState.button == config->button1 || keyState.button == config->button2)
		{
			// Two buttons
			if (config->button1 != -1)
			{
				if (isHolding1 || isHolding2)
				{
					toggle_clicking(keyState.evtype);
				}
			}
			// One button
			else
			{
				toggle_clicking(keyState.evtype);
			}

			isHolding1 = keyState.button == config->button1 && keyState.evtype == KeyPress;
			isHolding2 = keyState.button == config->button2 && keyState.evtype == KeyPress;
		}
	}

	XCloseDisplay(display);
}

void set_start_stop_button_hotkey_text()
{
	Display *display = get_display();
	const char *start_text_1 = "Start";
	const char *stop_text_1 = "Stop";

	// Button2 should always be defined
	const char *button_2_key = keycode_to_string(display, config->button2);

	char *start_text;
	char *stop_text;

	// If 2 keys
	if (config->button1 != -1)
	{
		const char *button_1_key = keycode_to_string(display, config->button1);
		start_text = malloc(1 + strlen(start_text_1) + strlen(button_1_key) + strlen(button_2_key) + 4);
		stop_text = malloc(1 + strlen(stop_text_1) + strlen(button_1_key) + strlen(button_2_key) + 4);
		sprintf(start_text, "%s (%s+%s)", start_text_1, button_1_key, button_2_key);
		sprintf(stop_text, "%s (%s+%s)", stop_text_1, button_1_key, button_2_key);
	}
	// Only one key
	else
	{
		start_text = malloc(1 + strlen(start_text_1) + strlen(button_2_key) + 3);
		stop_text = malloc(1 + strlen(stop_text_1) + strlen(button_2_key) + 3);
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

/**
 * Toggles the random interval textbox.
 */
void random_interval_check_toggle(GtkToggleButton *self)
{
	gtk_widget_set_sensitive(mainappwindow.random_interval_entry, gtk_toggle_button_get_active(self));

	g_key_file_set_boolean(config_gfile, PCK_RANDOM_INTERVAL, gtk_toggle_button_get_active(self));
	save_and_populate_config();
}

/**
 * Toggles the hold_time textbox.
 */
void hold_time_check_toggle(GtkToggleButton *self)
{
	gtk_widget_set_sensitive(mainappwindow.hold_time_entry, gtk_toggle_button_get_active(self));
	gtk_widget_set_sensitive(mainappwindow.holdtime_type_entry, gtk_toggle_button_get_active(self));

	g_key_file_set_boolean(config_gfile, PCK_HOLD_TIME, gtk_toggle_button_get_active(self));
	save_and_populate_config();
}

void holdtime_type_entry_changed()
{
	const gchar *holdtime_type_text = gtk_entry_get_text(GTK_ENTRY(mainappwindow.holdtime_type_entry));

	g_key_file_set_string(config_gfile, PCK_HOLD_TIME_TYPE, holdtime_type_text);
	g_key_file_save_to_file(config_gfile, configpath, NULL);
}
/**
 * Loads template, configuration, keybinds.
 * Sets all mainappwindow values to binded win values.
 */
static void main_app_window_init(MainAppWindow *win)
{
	set_window_icon(win);

	gtk_widget_init_template(GTK_WIDGET(win));
	config_init();

	mainappwindow.pwin = gtk_widget_get_toplevel(win);

	// Entries
	mainappwindow.hours_entry = win->hours_entry;
	mainappwindow.minutes_entry = win->minutes_entry;
	mainappwindow.seconds_entry = win->seconds_entry;
	mainappwindow.millisecs_entry = win->millisecs_entry;
	mainappwindow.repeat_entry = win->repeat_entry;
	mainappwindow.mouse_entry = win->mouse_entry;
	mainappwindow.click_type_entry = win->click_type_entry;
	mainappwindow.x_entry = win->x_entry;
	mainappwindow.y_entry = win->y_entry;
	mainappwindow.random_interval_entry = win->random_interval_entry;
	mainappwindow.hotkey_type_entry = win->hotkey_type_entry;
	mainappwindow.hold_time_entry = win->hold_time_entry;
	mainappwindow.holdtime_type_entry = win->holdtime_type_entry;

	// Checkboxes
	mainappwindow.repeat_only_check = win->repeat_only_check;
	mainappwindow.custom_location_check = win->custom_location_check;
	mainappwindow.random_interval_check = win->random_interval_check;
	mainappwindow.hold_time_check = win->hold_time_check;

	// Buttons
	mainappwindow.start_button = win->start_button;
	mainappwindow.stop_button = win->stop_button;
	mainappwindow.settings_button = win->settings_button;
	mainappwindow.get_button = win->get_button;

	set_start_stop_button_hotkey_text();
	mainappwindow_import_config();

	g_thread_new("get_start_stop_key_handler", get_start_stop_key_handler, NULL);
}

/**
 * Initialize the window class.
 * Binds template childs and callbacks to functions and values.
 */
static void main_app_window_class_init(MainAppWindowClass *class)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/res/ui/xclicker-window.ui");
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), insert_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), start_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), stop_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), click_type_entry_changed);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), mouse_button_entry_changed);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), hotkey_type_entry_changed);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), repeat_only_check_toggle);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), input_changed_save_handler);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), settings_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), custom_location_check_toggle);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), get_button_clicked);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), random_interval_check_toggle);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), hold_time_check_toggle);
	gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), holdtime_type_entry_changed);

	// Entries
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hours_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, minutes_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, seconds_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, millisecs_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, repeat_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, mouse_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, click_type_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, x_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, y_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, random_interval_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hotkey_type_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hold_time_entry);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, holdtime_type_entry);

	// Checkboxes
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, repeat_only_check);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, custom_location_check);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, random_interval_check);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, hold_time_check);

	// Buttons
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, start_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, stop_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, settings_button);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), MainAppWindow, get_button);
}

/**
 * Open up the main_app_window.
 */
MainAppWindow *main_app_window_new(XClickerApp *app)
{
	return g_object_new(MAIN_APP_WINDOW_TYPE, "application", app, NULL);
}
