#include <gtk/gtk.h>
#include <pwd.h>
#include "macros.h"

const char *configpath;
GKeyFile *config;

const char *get_config_file_path()
{
    const char *config = "/.config/xclicker.conf";
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL)
        homedir = getpwuid(getuid())->pw_dir;
    char *configfile = (char *)malloc(1 + strlen(homedir) + strlen(config));
    strcpy(configfile, homedir);
    strcat(configfile, config);
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

// Handlers
void safe_mode_changed(GtkSwitch *self, gboolean state)
{
    g_key_file_set_boolean(config, "Options", "SAFEMODE", state);
    g_key_file_save_to_file(config, configpath, NULL);
    // Hack to make the background color not glitch
    gtk_switch_set_active(self, state);
}

void config_init()
{
    configpath = get_config_file_path();
    config = get_config_keyfile(configpath);
}

void settings_dialog_new()
{
    GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/xclicker/ui/settings-dialog.ui");
    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

    gtk_builder_add_callback_symbol(builder, "safe_mode_changed", safe_mode_changed);
    gtk_builder_connect_signals(builder, NULL);

    gtk_switch_set_active(GTK_SWITCH(gtk_builder_get_object(builder, "safe_mode_switch")), is_safemode());

    gtk_dialog_run(dialog);
    // Else you will have to click twice to close the dialog
    gtk_widget_destroy(GTK_WIDGET(dialog));
}