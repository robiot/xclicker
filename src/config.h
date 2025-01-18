#ifndef __CONFIG_H
#define __CONFIG_H
#include <gtk/gtk.h>

/**
 * Represents all options you can see when starting xclicker.
 * kindof sus atm because the options are updated live,
 * but other options are just updated on start and whenever you open settings
 * please dont use config->(anything other than options) unless it is for initialisation
 */
struct Config
{
	/// options
	int button1;
	int button2;
	gboolean safe_mode_enabled;
	gboolean use_xevent;

	/// click interval
	const char *hours;
	const char *minutes;
	const char *seconds;
	const char *millisecs;

	/// opitons
	const char *mouse_button;
	const char *click_type;
	const char *hotkey;
	gboolean use_repeat;
	const char *repeat_times;

	/// more options
	gboolean use_custom_location;
	const char *custom_x;
	const char *custom_y;
	gboolean use_random_interval;
	const char *random_interval_ms;
	gboolean use_hold_time;
	const char *hold_time_ms;
	const char *holdtime_type;
};

extern const char *configpath;
extern GKeyFile *config_gfile;
extern struct Config *config;

#define CONFIG_CATEGORY_OPTIONS "Options"
#define PRESET_CATEGORY_CLICK_INTERVAL "Preset.Click Interval"
#define PRESET_CATEGORY_OPTIONS "Preset.Options"
#define PRESET_CATEGORY_MORE_OPTIONS "Preset.More Options"

// config keys
#define CFGK_BUTTON_1 CONFIG_CATEGORY_OPTIONS, "BUTTON1"
#define CFGK_BUTTON_2 CONFIG_CATEGORY_OPTIONS, "BUTTON2"
#define CFGK_USE_XEVENT CONFIG_CATEGORY_OPTIONS, "USE_XEVENT"
#define CFGK_SAFEMODE CONFIG_CATEGORY_OPTIONS, "SAFEMODE"

/// PCK stand for preset-category-key. Below set of defines used to prevent typos
#define PCK_HOURS PRESET_CATEGORY_CLICK_INTERVAL, "Hours"
#define PCK_MINUTES PRESET_CATEGORY_CLICK_INTERVAL, "Minutes"
#define PCK_SECONDS PRESET_CATEGORY_CLICK_INTERVAL, "Seconds"
#define PCK_MILLISECS PRESET_CATEGORY_CLICK_INTERVAL, "Millisecs"

#define PCK_MOUSE_BUTTON PRESET_CATEGORY_OPTIONS, "Millisecs"
#define PCK_CLICK_TYPE PRESET_CATEGORY_OPTIONS, "Click Type"
#define PCK_HOTKEY PRESET_CATEGORY_OPTIONS, "Hotkey"
#define PCK_REPEAT PRESET_CATEGORY_OPTIONS, "Use Repeat"
#define PCK_REPEAT_TIMES PRESET_CATEGORY_OPTIONS, "Repeat Times"

#define PCK_CUSTOM_LOCATION PRESET_CATEGORY_MORE_OPTIONS, "Use Custom Location"
#define PCK_CUSTOM_X PRESET_CATEGORY_MORE_OPTIONS, "Custom X"
#define PCK_CUSTOM_Y PRESET_CATEGORY_MORE_OPTIONS, "Custom Y"
#define PCK_RANDOM_INTERVAL PRESET_CATEGORY_MORE_OPTIONS, "Use Random Interval"
#define PCK_RANDOM_INTERVAL_MS PRESET_CATEGORY_MORE_OPTIONS, "Random Interval ms"
#define PCK_HOLD_TIME PRESET_CATEGORY_MORE_OPTIONS, "Use Hold Time"
#define PCK_HOLD_TIME_MS PRESET_CATEGORY_MORE_OPTIONS, "Hold Time ms"
#define PCK_HOLD_TIME_TYPE PRESET_CATEGORY_MORE_OPTIONS, "Hold Time Type"

void config_init();

void save_and_populate_config();

/**
 * Checks if configured to use safe-mode.
 */
gboolean is_safemode();

/**
 * Read config values from the config.
 */
struct Config *config_read_from_file();
#endif
