#ifndef __X11_H
#define __X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

enum MaskFlags
{
    MASK_KEYBOARD_PRESS = 1 << 0,
    MASK_KEYBOARD_RELEASE = 1 << 1,
    MASK_MOUSE_PRESS = 1 << 2
};

enum ClickModes
{
    CLICK_MODE_XTEST,
    CLICK_MODE_XEVENT,
};

/**
 * @brief Enumeration of the supported mouse events.
 * @see mouse_event
 */
enum MouseEvents
{
    MOUSE_EVENT_PRESS,
    MOUSE_EVENT_RELEASE,
};

/**
 * Configure XInput masks for the given display.
 * @param display The display to configure, get with get_display()
 * @param mode The mode to configure for [MASK_CONFIG_MOUSE/MASK_CONFIG_KEYBOARD]
 * @see get_display
 */
void mask_config(Display *display, enum MaskFlags flags);

/**
 * @brief Struct that get_next_key_state returns
 * @see get_next_key_state
 */
typedef struct _KeyState
{
    int button;
    int evtype;
} KeyState;

/**
 * Waits until key pressed.
 * @returns The KeyState in the buffer given
 */
void get_next_key_state(Display *display, KeyState *buffer);

/**
 * Get the cursors current location.
 * @returns Cursor X,Y to the given int pointers
 */
void get_cursor_coords(Display *display, int *x, int *y);

/**
 * Move the cursor to given absolute location
 */
void move_to(Display *display, int x, int y);

/**
 * Mouse event on current mouse position with given button.
 * There is two modes, xtest and xevent. Xevent is generally more safe since it doesn't interact with gnome-shell, titlebars and gtk_applications.
 * Xevent also only clicks the currently focused application.
 * XTest is like mouse_event on windows. It focuses the application the cursor is hovering
 * over and clicks on everything that xevent can't plus what xevent can.
 * @param mode The mode to use to click [CLICK_MODE_XTEST/CLICK_MODE_XEVENT]
 */
int mouse_event(Display *display, int button, int mode, enum MouseEvents event_type);

/**
 * Click on current mouse position with given button.
 * There is two modes, xtest and xevent. Xevent is generally more safe since it doesn't interact with gnome-shell, titlebars and gtk_applications.
 * Xevent also only clicks the currently focused application.
 * XTest is like mouse_event on windows. It focuses the application the cursor is hovering
 * over and clicks on everything that xevent can't plus what xevent can.
 * @param mode The mode to use to click [CLICK_MODE_XTEST/CLICK_MODE_XEVENT]
 */
int click(Display *display, int button, int mode, int sleep);

/**
 * Converts a given x11 keycode to a string
 * which is not always just one character (ex shift).
 */
char *keycode_to_string(Display *display, int keycode);

/**
 * Gets a new display.
 * Use XCloseDisplay after use.
 */
Display *get_display();

#endif
