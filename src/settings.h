#ifndef __SETTINGSDIALOG_H
#define __SETTINGSDIALOG_H

extern int button1;
extern int button2;
extern gboolean isChoosingHotkey;

/**
 * Checks if configured to use safe-mode.
*/
gboolean is_safemode();

/**
 * Checks if configured to use xevent.
*/
gboolean is_using_xevent();

/**
 * Loads config file if exists, else create it.
*/
void config_init();

void load_start_stop_keybinds();

/**
 * Opens up a new settings dialog.
 */
void settings_dialog_new();

#endif