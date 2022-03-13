#ifndef __X11_H
#define __X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define MASK_CONFIG_MOUSE (0)
#define MASK_CONFIG_KEYBOARD (1)

#define CLICK_MODE_XTEST (0) // Default
#define CLICK_MODE_XEVENT (1)

/**
 * Configure XInput masks for the given display.
 * @param display The display to configure, get with get_display()
 * @param mode The mode to configure for [MASK_CONFIG_MOUSE/MASK_CONFIG_KEYBOARD]
 * @see get_display
 */
void mask_config(Display *display, int mode);

/**
 * Waits until key pressed.
 * @returns The keycode for the pressed key
 */
int get_next_key_state(Display *display);

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
 * Mouse down on current mouse position with given button.
 * There is two modes, xtest and xevent. Xevent is generally more safe since it doesn't interact with gnome-shell, titlebars and gtk_applications.
 * Xevent also only clicks the currently focused application.
 * XTest is like mouse_event on windows. It focuses the application the cursor is hovering
 * over and clicks on everything that xevent can't plus what xevent can.
 * @param mode The mode to use to click [CLICK_MODE_XTEST/CLICK_MODE_XEVENT]
 */
int mouse_down(Display *display, int button, int mode);

/**
 * Mouse up on current mouse position with given button.
 * There is two modes, xtest and xevent. Xevent is generally more safe since it doesn't interact with gnome-shell, titlebars and gtk_applications.
 * Xevent also only clicks the currently focused application.
 * XTest is like mouse_event on windows. It focuses the application the cursor is hovering
 * over and clicks on everything that xevent can't plus what xevent can.
 * @param mode The mode to use to click [CLICK_MODE_XTEST/CLICK_MODE_XEVENT]
 */
int mouse_up(Display *display, int button, int mode);

/**
 * Click on current mouse position with given button.
 * There is two modes, xtest and xevent. Xevent is generally more safe since it doesn't interact with gnome-shell, titlebars and gtk_applications.
 * Xevent also only clicks the currently focused application.
 * XTest is like mouse_event on windows. It focuses the application the cursor is hovering
 * over and clicks on everything that xevent can't plus what xevent can.
 * @param mode The mode to use to click [CLICK_MODE_XTEST/CLICK_MODE_XEVENT]
 */
int click(Display *display, int button, int mode);

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