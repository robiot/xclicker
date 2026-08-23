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

gboolean set_buttons_entry_text(gpointer data)
{
    char *text = data;

    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), text);
    g_free(text);

    return FALSE;
}

/**
 * Shows the given text in the hotkey entry. Takes ownership of `text`.
 */
void set_buttons_entry_text_async(char *text)
{
    g_idle_add(set_buttons_entry_text, text);
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
 * Compares a keycode against a keysym.
 * A keysym that is not in the current layout resolves to keycode 0, which must
 * not be treated as a match, otherwise every unmapped keysym in the lists
 * below matches whatever else happens to be 0.
 */
gboolean keycode_is(Display *display, int keycode, KeySym keysym)
{
    const KeyCode mapped = XKeysymToKeycode(display, keysym);

    return mapped != 0 && keycode == mapped;
}

/**
 * Keys that are skipped instead of being bound.
 * Numlock & caps lock is incredibly buggy and causes memory leaks, pointer errors, free errors...
 */
gboolean is_ignored_key(Display *display, int keycode)
{
    return keycode_is(display, keycode, XK_Num_Lock) || keycode_is(display, keycode, XK_Caps_Lock);
}

/**
 * Keys that only make up the first half of a combination, ex shift, ctrl.
 */
gboolean is_prekey(Display *display, int keycode)
{
    static const KeySym prekeys[] = {
        XK_Shift_L, XK_Shift_R, XK_Alt_L, XK_Alt_R, XK_Escape,
        XK_Control_L, XK_Control_R, XK_ISO_Level3_Shift, XK_Super_L, XK_Super_R};

    for (gsize i = 0; i < G_N_ELEMENTS(prekeys); i++)
    {
        if (keycode_is(display, keycode, prekeys[i]))
            return TRUE;
    }

    return FALSE;
}

/**
 * Gets the keys or mouse buttons pressed when setting hotkey.
 */
void get_hotkeys_handler()
{
    Display *display = get_display();
    mask_config(display, MASK_KEYBOARD_PRESS | MASK_MOUSE_PRESS);

    // Clicks xclicker sends itself are not the user choosing a hotkey,
    // which matters because settings can be opened while it is clicking.
    const int xtest_pointer = get_xtest_pointer_deviceid(display);

    int prekey = -1;

    while (1)
    {
        KeyState keyState;

        /*
         * Anything that is not an actual press is skipped. Without this a
         * keymap change or an auto-repeat gets decoded as keycode 0 and stored
         * as the hotkey, so the loop ends before the user ever pressed a key.
         */
        if (!get_next_key_state(display, &keyState) || !keyState.pressed)
            continue;

        if (keyState.type == INPUT_TYPE_BUTTON && keyState.sourceid == xtest_pointer)
            continue;

        if (keyState.type == INPUT_TYPE_KEY && is_ignored_key(display, keyState.button))
            continue;

        if (keyState.type == INPUT_TYPE_KEY && is_prekey(display, keyState.button))
        {
            prekey = keyState.button;

            char *prekey_str = input_to_string(display, prekey, INPUT_TYPE_KEY);
            set_buttons_entry_text_async(g_strdup_printf("%s + ", prekey_str));
            g_free(prekey_str);

            continue;
        }

        // Anything else finishes the hotkey
        config->button1 = prekey;
        config->button1_type = INPUT_TYPE_KEY;
        config->button2 = keyState.button;
        config->button2_type = keyState.type;

        char *key_str = input_to_string(display, keyState.button, keyState.type);

        if (prekey != -1)
        {
            char *prekey_str = input_to_string(display, prekey, INPUT_TYPE_KEY);
            set_buttons_entry_text_async(g_strdup_printf("%s + %s", prekey_str, key_str));
            g_free(prekey_str);
        }
        else
        {
            set_buttons_entry_text_async(g_strdup(key_str));
        }

        g_free(key_str);
        break;
    }

    XCloseDisplay(display);
    g_idle_add(enable_start_button, NULL);
    g_idle_add(hotkey_finished, NULL);

    g_key_file_set_integer(config_gfile, CFGK_BUTTON_1, config->button1);
    g_key_file_set_integer(config_gfile, CFGK_BUTTON_2, config->button2);
    g_key_file_set_integer(config_gfile, CFGK_BUTTON_1_TYPE, config->button1_type);
    g_key_file_set_integer(config_gfile, CFGK_BUTTON_2_TYPE, config->button2_type);

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
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), "Press Desired Keys or Mouse Buttons");
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
    char *button_2_key = input_to_string(display, config->button2, config->button2_type);
    char *hotkeys;

    if (config->button1 != -1)
    {
        char *button_1_key = input_to_string(display, config->button1, config->button1_type);
        hotkeys = g_strdup_printf("%s + %s", button_1_key, button_2_key);
        g_free(button_1_key);
    }
    else
    {
        hotkeys = g_strdup(button_2_key);
    }
    gtk_entry_set_text(GTK_ENTRY(items.buttons_entry), hotkeys);

    g_free(button_2_key);
    g_free(hotkeys);
    XCloseDisplay(display);

    // Run
    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
