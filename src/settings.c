#include <gtk/gtk.h>
#include <pwd.h>
#include <X11/keysymdef.h>
#include <sys/stat.h>
#include "settings.h"
#include "x11api.h"
#include "mainwin.h"
#include "version.h"

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
    const char *config = "/.config";
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL)
        homedir = getpwuid(getuid())->pw_dir;
    char *configpath = (char *)malloc(1 + strlen(homedir) + strlen(config));
    strcpy(configpath, homedir);
    strcat(configpath, config);
    mkdir(configpath, 0777);

    char *configfile = (char *)malloc(1 + strlen(configpath) + strlen("/xclicker.conf"));
    strcpy(configfile, configpath);
    strcat(configfile, "/xclicker.conf");
    free(configpath);
    return configfile;
}

GKeyFile *get_config_keyfile(const char *configpath)
{
    GKeyFile *config = g_key_file_new();
    if (!g_key_file_load_from_file(config, configpath, G_KEY_FILE_KEEP_COMMENTS, NULL))
        g_print("Could not load config file. This could mean that you never changed any settings or that the config is corrupted.\n");
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
    GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/xclicker/ui/settings-dialog.ui");
    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

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
