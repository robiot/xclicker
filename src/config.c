#include <gtk/gtk.h>
#include "config.h"
#include "x11api.h"

const char *configpath;
GKeyFile *config_gfile;

// The config struct
struct Config *config;

const char *get_config_file_path()
{
    const char *config_path = g_get_user_config_dir();
    const char *file_name = "/xclicker.conf";

    if (!opendir(config_path))
    {
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

void config_init()
{
    Display *display = get_display();

    configpath = get_config_file_path();
    config_gfile = get_config_keyfile(configpath);

    config = config_read_from_file();

    XCloseDisplay(display);
}

void save_and_populate_config()
{
    g_key_file_save_to_file(config_gfile, configpath, NULL);
    config = config_read_from_file();
}

gboolean is_safemode()
{
    if (config->safe_mode_enabled == TRUE)
        return TRUE;
    if (access(configpath, F_OK))
        return TRUE;

    return FALSE;
}

void load_start_stop_keybinds(struct Config *config)
{
    Display *display = get_display();
    const int button_1 = g_key_file_get_integer(config_gfile, CFGK_BUTTON_1, NULL);
    const int button_2 = g_key_file_get_integer(config_gfile, CFGK_BUTTON_2, NULL);

    // Initial values
    config->button1 = -1;
    config->button2 = XKeysymToKeycode(display, XK_F8);

    if (button_1 != 0 && button_1)
        config->button1 = button_1;

    if (button_2 != 0 && button_2)
        config->button2 = button_2;

    XCloseDisplay(display);
}

struct Config *config_read_from_file()
{
    struct Config *config = g_malloc0(sizeof(*config));

    // config->button1 = g_key_file_get_integer(config_gfile, CFGK_BUTTON_1, NULL);
    // config->button2 = g_key_file_get_integer(config_gfile, CFGK_BUTTON_2, NULL);
    load_start_stop_keybinds(config); // Makes it work

    config->safe_mode_enabled = g_key_file_get_boolean(config_gfile, CFGK_SAFEMODE, NULL);
    config->use_xevent = g_key_file_get_boolean(config_gfile, CFGK_USE_XEVENT, NULL);

    config->hours = g_key_file_get_string(config_gfile, PCK_HOURS, NULL);
    config->minutes = g_key_file_get_string(config_gfile, PCK_MINUTES, NULL);
    config->seconds = g_key_file_get_string(config_gfile, PCK_SECONDS, NULL);
    config->millisecs = g_key_file_get_string(config_gfile, PCK_MILLISECS, NULL);
    if (!config->millisecs)
    {
        config->millisecs = "100";
    }

    config->mouse_button = g_key_file_get_string(config_gfile, PCK_MOUSE_BUTTON, NULL);
    if (!config->mouse_button)
    {
        config->mouse_button = "Left";
    }

    config->click_type = g_key_file_get_string(config_gfile, PCK_CLICK_TYPE, NULL);
    if (!config->click_type)
    {
        config->click_type = "Single";
    }

    config->hotkey = g_key_file_get_string(config_gfile, PCK_HOTKEY, NULL);
    if (!config->hotkey)
    {
        config->hotkey = "Normal";
    }

    config->holdtime_type = g_key_file_get_string(config_gfile, PCK_HOLD_TIME_TYPE, NULL);
    if (!config->holdtime_type)
    {
        config->holdtime_type = "Constant";
    }

    config->use_repeat = g_key_file_get_boolean(config_gfile, PCK_REPEAT, NULL);
    config->repeat_times = g_key_file_get_string(config_gfile, PCK_REPEAT_TIMES, NULL);

    config->use_custom_location = g_key_file_get_boolean(config_gfile, PCK_CUSTOM_LOCATION, NULL);
    config->custom_x = g_key_file_get_string(config_gfile, PCK_CUSTOM_X, NULL);
    config->custom_y = g_key_file_get_string(config_gfile, PCK_CUSTOM_Y, NULL);
    config->use_random_interval = g_key_file_get_boolean(config_gfile, PCK_RANDOM_INTERVAL, NULL);
    config->random_interval_ms = g_key_file_get_string(config_gfile, PCK_RANDOM_INTERVAL_MS, NULL);
    config->use_hold_time = g_key_file_get_boolean(config_gfile, PCK_HOLD_TIME, NULL);
    config->hold_time_ms = g_key_file_get_string(config_gfile, PCK_HOLD_TIME_MS, NULL);

    return config;
}
