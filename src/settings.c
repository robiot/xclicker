#include <gtk/gtk.h>
#include <pwd.h>
#include <X11/keysymdef.h>
#include <dirent.h>
#include <unistd.h>

#include "settings.h"
#include "x11api.h"
#include "mainwin.h"
#include "version.h"
#include "utils.h"

// Default values, later set by config_init
int button1 = -1;
int button2 = 74;

gboolean isChoosingHotkey = FALSE;
const char *configpath;
GKeyFile *config;

struct _items
{
    GtkWidget *buttons_entry;
    GtkWidget *start_button;
    GtkWidget *xevent_switch;
} items;

const char *get_config_file_path()
{
    const char *config_path = g_get_user_config_dir();
    const char *file_name = "/xclicker.conf";

    if (!opendir(config_path)) {
        g_printerr("Could not find any path to get or store the settings in.\n");
        return NULL;
    }

    char *config_file_path = malloc(strlen(config_path) + strlen(file_name) + 1);
    strcpy(config_file_path, config_path);
    strcat(config_file_path, "/xclicker.conf");

    return config_file_path;
}

GKeyFile *get_config_keyfile(const char *config_path)
{
    GKeyFile *config = g_key_file_new();

    // Reversed boolean
    if (access(config_path, F_OK) == 0 && !g_key_file_load_from_file(config, config_path, G_KEY_FILE_KEEP_COMMENTS, NULL))
        g_print("The config file seems to be corrupted.\n");

    return config;
}

gboolean is_safemode()
{
    if (g_key_file_get_boolean(config, "Options", "SAFEMODE", NULL) == TRUE)
        return TRUE;
    if (access(configpath, F_OK))
        return TRUE;

    return FALSE;
}

gboolean is_using_xevent()
{
    return g_key_file_get_boolean(config, "Options", "USE_XEVENT", NULL);
}

void config_init()
{
    Display *display = get_display();
    button1 = -1;
    button2 = XKeysymToKeycode(display, XK_F8);
    configpath = get_config_file_path();
    config = get_config_keyfile(configpath);
    XCloseDisplay(display);
}

void load_start_stop_keybinds()
{
    Display *display = get_display();
    const int button_1 = g_key_file_get_integer(config, "Options", "BUTTON1", NULL);
    const int button_2 = g_key_file_get_integer(config, "Options", "BUTTON2", NULL);

    if (button_1 != 0 && button_1)
        button1 = button_1;

    if (button_2 != 0 && button_2)
        button2 = button_2;

    XCloseDisplay(display);
}

struct set_buttons_entry_struct
{
	char *text;
};

void set_buttons_entry_text(gpointer *data)
{
	struct set_buttons_entry_struct *args = data;
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), args->text);
    free(args->text);
    g_free(args);
}

void enable_start_button()
{
    gtk_widget_set_sensitive(items.start_button, TRUE);
}

void hotkey_finished()
{
    set_start_stop_button_hotkey_text();
}

/**
 * Gets the keys pressed when setting hotkey.
 */
void get_hotkeys_handler()
{
    Display *display = get_display();
    mask_config(display, MASK_KEYBOARD_PRESS);

    gboolean hasPreKey = FALSE;
    while (1)
    {
        KeyState keyState;
        get_next_key_state(display, &keyState);

        int state = keyState.button;

        // Numlock & caps lock is incredibly buggy and causes memory leaks, pointer errors, free errors...
        if (state == XKeysymToKeycode(display, XK_Num_Lock) || state == XKeysymToKeycode(display, XK_Caps_Lock))
            continue;

        // If prekey, ex shift, ctrl
        if (state == XKeysymToKeycode(display, XK_Shift_L) || state == XKeysymToKeycode(display, XK_Shift_R) 
            || state == XKeysymToKeycode(display, XK_Alt_L) || state == XKeysymToKeycode(display, XK_Alt_R)
            || state == XKeysymToKeycode(display, XK_Escape) || state == XKeysymToKeycode(display, XK_Control_L)
            || state == XKeysymToKeycode(display, XK_Control_R) || state == XKeysymToKeycode(display, XK_ISO_Level3_Shift)
            || state == XKeysymToKeycode(display, XK_Super_L) || state == XKeysymToKeycode(display, XK_Super_R))
        {
            hasPreKey = TRUE;
            button1 = state;
            const char *key_str = keycode_to_string(display, state);
            const char *plus = " + ";
            char *text = malloc(strlen(key_str) + strlen(plus));
            sprintf(text, "%s%s", key_str, plus);
            
            struct set_buttons_entry_struct *user_data = g_malloc0(sizeof(struct set_buttons_entry_struct));
		    user_data->text = text;
            g_idle_add(set_buttons_entry_text, user_data);
        }
        else 
        {
            button2 = state;
            const char *key_str = keycode_to_string(display, state);
            struct set_buttons_entry_struct *user_data = g_malloc0(sizeof(struct set_buttons_entry_struct));
            

            if (hasPreKey == TRUE)
            {
                const char *buttons_entry_text = gtk_entry_get_text(GTK_ENTRY(items.buttons_entry));
                char *text = malloc(1 + strlen(buttons_entry_text) + strlen(key_str));
                sprintf(text, "%s%s", buttons_entry_text, key_str);
                user_data->text = text;
            }
            else 
            {
                button1 = -1;
                char *text = (char *)malloc(1 + strlen(key_str));
                strcpy(text, key_str);
                user_data->text = text;
            }
            // Text is freed in the set_buttons_entry_text function
            g_idle_add(set_buttons_entry_text, user_data);
            break;
        }
    }
    XCloseDisplay(display);
    g_idle_add(enable_start_button, NULL);
    g_idle_add(hotkey_finished, NULL);

    g_key_file_set_integer(config, "Options", "BUTTON1", button1);
    g_key_file_set_integer(config, "Options", "BUTTON2", button2);
    g_key_file_save_to_file(config, configpath, NULL);
    isChoosingHotkey = FALSE;
}


void safe_mode_changed(GtkSwitch *self, gboolean state)
{
    g_key_file_set_boolean(config, "Options", "SAFEMODE", state);
    g_key_file_save_to_file(config, configpath, NULL);
    // Hack to make the background color not glitch
    gtk_switch_set_active(self, state);
}

void xevent_switch_changed(GtkSwitch *self, gboolean state)
{
    g_key_file_set_boolean(config, "Options", "USE_XEVENT", state);
    g_key_file_save_to_file(config, configpath, NULL);
    gtk_switch_set_active(self, state);
}

void start_button_pressed(GtkButton *self)
{
    isChoosingHotkey = TRUE;
    gtk_widget_set_sensitive(GTK_WIDGET(self), FALSE);
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), "Press Desired Keys");
    g_thread_new("get_hotkeys_handler", get_hotkeys_handler, NULL);
}

void settings_dialog_new()
{
    GtkBuilder *builder = gtk_builder_new_from_resource("/res/ui/settings-dialog.ui");
    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	set_window_icon(dialog);

    gtk_builder_add_callback_symbol(builder, "safe_mode_changed", safe_mode_changed);
    gtk_builder_add_callback_symbol(builder, "xevent_switch_changed", xevent_switch_changed);
    gtk_builder_add_callback_symbol(builder, "start_button_pressed", start_button_pressed);
    gtk_builder_connect_signals(builder, NULL);

    // Load version
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "version_label")), XCLICKER_VERSION);

    // Fill struct
    items.buttons_entry = gtk_builder_get_object(builder, "buttons_entry");
    items.start_button = gtk_builder_get_object(builder, "start_button");
    items.xevent_switch = gtk_builder_get_object(builder, "xevent_switch");

    // Load
    gtk_switch_set_active(GTK_SWITCH(gtk_builder_get_object(builder, "safe_mode_switch")), is_safemode());
    gtk_switch_set_active(GTK_SWITCH(items.xevent_switch), is_using_xevent());

    // Load hotkeys
    Display *display = get_display();
    const char *button_2_key = keycode_to_string(display, button2);
    const char *sep = " + ";
    char *hotkeys;

    if (button1 != -1)
    {
        const char *button_1_key = keycode_to_string(display, button1);
        hotkeys = malloc(strlen(sep) + strlen(button_2_key) + strlen(button_1_key));
        sprintf(hotkeys, "%s%s%s", button_1_key, sep, button_2_key);
    }
    else
    {
        hotkeys = malloc(strlen(button_2_key));
        sprintf(hotkeys, "%s", button_2_key);
    }
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), hotkeys);

    free(hotkeys);
    XCloseDisplay(display);

    // Run
    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

#define PRESET_CATEGORY_CLICK_INTERVAL "Preset.Click Interval"
#define PRESET_CATEGORY_OPTIONS        "Preset.Options"
#define PRESET_CATEGORY_MORE_OPTIONS   "Preset.More Options"

/// PCK stand for preset-category-key. Below set of defines used to prevent typos
#define PCK_HOURS              PRESET_CATEGORY_CLICK_INTERVAL, "Hours"
#define PCK_MINUTES            PRESET_CATEGORY_CLICK_INTERVAL, "Minutes"
#define PCK_SECONDS            PRESET_CATEGORY_CLICK_INTERVAL, "Seconds"
#define PCK_MILLISECS          PRESET_CATEGORY_CLICK_INTERVAL, "Millisecs"

#define PCK_MOUSE_BUTTON       PRESET_CATEGORY_OPTIONS, "Millisecs"
#define PCK_CLICK_TYPE         PRESET_CATEGORY_OPTIONS, "Click Type"
#define PCK_HOTKEY             PRESET_CATEGORY_OPTIONS, "Hotkey"
#define PCK_REPEAT             PRESET_CATEGORY_OPTIONS, "Use Repeat"
#define PCK_REPEAT_TIMES       PRESET_CATEGORY_OPTIONS, "Repeat Times"

#define PCK_CUSTOM_LOCATION    PRESET_CATEGORY_MORE_OPTIONS, "Use Custom Location"
#define PCK_CUSTOM_X           PRESET_CATEGORY_MORE_OPTIONS, "Custom X"
#define PCK_CUSTOM_Y           PRESET_CATEGORY_MORE_OPTIONS, "Custom Y"
#define PCK_RANDOM_INTERVAL    PRESET_CATEGORY_MORE_OPTIONS, "Use Random Interval"
#define PCK_RANDOM_INTERVAL_MS PRESET_CATEGORY_MORE_OPTIONS, "Random Interval ms"
#define PCK_HOLD_TIME          PRESET_CATEGORY_MORE_OPTIONS, "Use Hold Time"
#define PCK_HOLD_TIME_MS       PRESET_CATEGORY_MORE_OPTIONS, "Hold Time ms"

void preset_write_to_config(struct Preset *preset)
{
	g_key_file_set_string (config, PCK_HOURS, preset->hours);
	g_key_file_set_string (config, PCK_MINUTES, preset->minutes);
	g_key_file_set_string (config, PCK_SECONDS, preset->seconds);
	g_key_file_set_string (config, PCK_MILLISECS, preset->millisecs);

	g_key_file_set_string (config, PCK_MOUSE_BUTTON, preset->mouse_button);
	g_key_file_set_string (config, PCK_CLICK_TYPE, preset->click_type);
	g_key_file_set_string (config, PCK_HOTKEY, preset->hotkey);
	g_key_file_set_boolean(config, PCK_REPEAT, preset->use_repeat);
	g_key_file_set_string (config, PCK_REPEAT_TIMES, preset->repeat_times);

	g_key_file_set_boolean(config, PCK_CUSTOM_LOCATION, preset->use_custom_location);
	g_key_file_set_string (config, PCK_CUSTOM_X, preset->custom_x);
	g_key_file_set_string (config, PCK_CUSTOM_Y, preset->custom_y);
	g_key_file_set_boolean(config, PCK_RANDOM_INTERVAL, preset->use_random_interval);
	g_key_file_set_string (config, PCK_RANDOM_INTERVAL_MS, preset->random_interval_ms);
	g_key_file_set_boolean(config, PCK_HOLD_TIME, preset->use_hold_time);
	g_key_file_set_string (config, PCK_HOLD_TIME_MS, preset->hold_time_ms);

	g_key_file_save_to_file(config, configpath, NULL);
	g_free(preset);
}

struct Preset *preset_read_from_config()
{
	struct Preset *preset = g_malloc0(sizeof(*preset));

	preset->hours                = g_key_file_get_string (config, PCK_HOURS, NULL);
	preset->minutes              = g_key_file_get_string (config, PCK_MINUTES, NULL);
	preset->seconds              = g_key_file_get_string (config, PCK_SECONDS, NULL);
	preset->millisecs            = g_key_file_get_string (config, PCK_MILLISECS, NULL);

	preset->mouse_button         = g_key_file_get_string (config, PCK_MOUSE_BUTTON, NULL);
	preset->click_type           = g_key_file_get_string (config, PCK_CLICK_TYPE, NULL);
	preset->hotkey               = g_key_file_get_string (config, PCK_HOTKEY, NULL);
	preset->use_repeat           = g_key_file_get_boolean(config, PCK_REPEAT, NULL);
	preset->repeat_times         = g_key_file_get_string (config, PCK_REPEAT_TIMES, NULL);

	preset->use_custom_location  = g_key_file_get_boolean(config, PCK_CUSTOM_LOCATION, NULL);
	preset->custom_x             = g_key_file_get_string (config, PCK_CUSTOM_X, NULL);
	preset->custom_y             = g_key_file_get_string (config, PCK_CUSTOM_Y, NULL);
	preset->use_random_interval  = g_key_file_get_boolean(config, PCK_RANDOM_INTERVAL, NULL);
	preset->random_interval_ms   = g_key_file_get_string (config, PCK_RANDOM_INTERVAL_MS, NULL);
	preset->use_hold_time        = g_key_file_get_boolean(config, PCK_HOLD_TIME, NULL);
	preset->hold_time_ms         = g_key_file_get_string (config, PCK_HOLD_TIME_MS, NULL);

	return preset;
}
