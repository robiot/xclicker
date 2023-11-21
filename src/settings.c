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
#include "config.h"

gboolean isChoosingHotkey = FALSE;

struct _items
{
    GtkWidget *buttons_entry;
    GtkWidget *start_button;
    GtkWidget *xevent_switch;
    GtkWidget *reset_preset_button;
} items;

struct set_buttons_entry_struct
{
    char *text;
};

gboolean set_buttons_entry_text(gpointer *data)
{
    struct set_buttons_entry_struct *args = data;
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), args->text);
    free(args->text);
    g_free(args);

    return FALSE;
}

gboolean enable_start_button()
{
    gtk_widget_set_sensitive(items.start_button, TRUE);
    return FALSE;
}

gboolean hotkey_finished()
{
    set_start_stop_button_hotkey_text();
    return FALSE;
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
        if (state == XKeysymToKeycode(display, XK_Shift_L) || state == XKeysymToKeycode(display, XK_Shift_R) || state == XKeysymToKeycode(display, XK_Alt_L) || state == XKeysymToKeycode(display, XK_Alt_R) || state == XKeysymToKeycode(display, XK_Escape) || state == XKeysymToKeycode(display, XK_Control_L) || state == XKeysymToKeycode(display, XK_Control_R) || state == XKeysymToKeycode(display, XK_ISO_Level3_Shift) || state == XKeysymToKeycode(display, XK_Super_L) || state == XKeysymToKeycode(display, XK_Super_R))
        {
            hasPreKey = TRUE;
            config->button1 = state;
            const char *key_str = keycode_to_string(display, state);
            const char *plus = " + ";
            char *text = malloc(1 + strlen(key_str) + strlen(plus));
            sprintf(text, "%s%s", key_str, plus);

            struct set_buttons_entry_struct *user_data = g_malloc0(sizeof(struct set_buttons_entry_struct));
            user_data->text = text;
            g_idle_add(set_buttons_entry_text, user_data);
        }
        else
        {
            config->button2 = state;
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
                config->button1 = -1;
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

    g_key_file_set_integer(config_gfile, CFGK_BUTTON_1, config->button1);
    g_key_file_set_integer(config_gfile, CFGK_BUTTON_2, config->button2);

    g_key_file_save_to_file(config_gfile, configpath, NULL);
    isChoosingHotkey = FALSE;
}

void safe_mode_changed(GtkSwitch *self, gboolean state)
{
    g_key_file_set_boolean(config_gfile, CFGK_SAFEMODE, state);
    config->safe_mode_enabled = state;

    g_key_file_save_to_file(config_gfile, configpath, NULL);
    // Hack to make the background color not glitch
    gtk_switch_set_active(self, state);
}

void xevent_switch_changed(GtkSwitch *self, gboolean state)
{
    g_key_file_set_boolean(config_gfile, CFGK_USE_XEVENT, state);

    save_and_populate_config();
    gtk_switch_set_active(self, state);
}

void start_button_pressed(GtkButton *self)
{
    isChoosingHotkey = TRUE;
    gtk_widget_set_sensitive(GTK_WIDGET(self), FALSE);
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), "Press Desired Keys");
    g_thread_new("get_hotkeys_handler", get_hotkeys_handler, NULL);
}

void reset_preset_button_pressed()
{
    g_key_file_remove_group(config_gfile, PRESET_CATEGORY_CLICK_INTERVAL, NULL);
    g_key_file_remove_group(config_gfile, PRESET_CATEGORY_OPTIONS, NULL);
    g_key_file_remove_group(config_gfile, PRESET_CATEGORY_MORE_OPTIONS, NULL);

    save_and_populate_config();
    mainappwindow_import_config();
}

void settings_dialog_new()
{
    GtkBuilder *builder = gtk_builder_new_from_resource("/res/ui/settings-dialog.ui");
    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

    config_read_from_file();

    set_window_icon(dialog);

    gtk_builder_add_callback_symbol(builder, "safe_mode_changed", safe_mode_changed);
    gtk_builder_add_callback_symbol(builder, "xevent_switch_changed", xevent_switch_changed);
    gtk_builder_add_callback_symbol(builder, "start_button_pressed", start_button_pressed);
    gtk_builder_add_callback_symbol(builder, "reset_preset_button_pressed", reset_preset_button_pressed);

    gtk_builder_connect_signals(builder, NULL);

    // Load version
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "version_label")), XCLICKER_VERSION);

    // Fill struct
    items.buttons_entry = gtk_builder_get_object(builder, "buttons_entry");
    items.start_button = gtk_builder_get_object(builder, "start_button");
    items.xevent_switch = gtk_builder_get_object(builder, "xevent_switch");

    // Load
    gtk_switch_set_active(GTK_SWITCH(gtk_builder_get_object(builder, "safe_mode_switch")), is_safemode());
    gtk_switch_set_active(GTK_SWITCH(items.xevent_switch), config->use_xevent);

    // Load hotkeys
    Display *display = get_display();
    const char *button_2_key = keycode_to_string(display, config->button2);
    const char *sep = " + ";
    char *hotkeys;

    if (config->button1 != -1)
    {
        const char *button_1_key = keycode_to_string(display, config->button1);
        hotkeys = malloc(1 + strlen(sep) + strlen(button_2_key) + strlen(button_1_key));
        sprintf(hotkeys, "%s%s%s", button_1_key, sep, button_2_key);
    }
    else
    {
        hotkeys = malloc(1 + strlen(button_2_key));
        sprintf(hotkeys, "%s", button_2_key);
    }
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), hotkeys);

    free(hotkeys);
    XCloseDisplay(display);

    // Run
    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
