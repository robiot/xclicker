#ifndef __SETTINGSDIALOG_H
#define __SETTINGSDIALOG_H

extern int button1;
extern int button2;
extern gboolean isChoosingHotkey;

gboolean is_safemode();
void config_init();
void load_start_stop_keybinds();
void settings_dialog_new();

#endif