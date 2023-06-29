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

/**
 * Represents all options you can see when starting xclicker.
 */
struct Preset
{
	/// click interval
	const char * hours;
	const char * minutes;
	const char * seconds;
	const char * millisecs;
	/// opitons
	const char * mouse_button;
	const char * click_type;
	const char * hotkey;
	gboolean use_repeat;
	const char * repeat_times;
	/// more options
	gboolean use_custom_location;
	const char * custom_x;
	const char * custom_y;
	gboolean use_random_interval;
	const char * random_interval_ms;
	gboolean use_hold_time;
	const char * hold_time_ms;
};

/**
 * Write preset values to config file and save it.
 */
void preset_write_to_config(struct Preset *preset);

/**
 * Read preset values from the config.
 */
struct Preset *preset_read_from_config();

#endif
