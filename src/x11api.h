#ifndef __X11_H
#define __X11_H

#include <glib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

enum MaskFlags
{
    MASK_KEYBOARD_PRESS = 1 << 0,
    MASK_KEYBOARD_RELEASE = 1 << 1,
    MASK_MOUSE_PRESS = 1 << 2,
    MASK_MOUSE_RELEASE = 1 << 3
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
 * @brief Whether an input refers to a keyboard key or a mouse button.
 * Needed because a keycode and a button number are both plain ints,
 * so ex. keycode 9 (Escape) and mouse button 9 have to be told apart.
 */
enum InputType
{
    INPUT_TYPE_KEY,
    INPUT_TYPE_BUTTON
};

/**
 * Configure XInput masks for the given display.
 * @param display The display to configure, get with get_display()
 * @param flags Any combination of MaskFlags
 * @see get_display
 */
void mask_config(Display *display, enum MaskFlags flags);

/**
 * @brief Struct that get_next_key_state returns
 * @see get_next_key_state
 */
typedef struct _KeyState
{
    /// Keycode when type is INPUT_TYPE_KEY, button number when INPUT_TYPE_BUTTON
    int button;
    /// The XI2 event type the state was decoded from
    int evtype;
    /// Whether `button` is a keyboard key or a mouse button
    enum InputType type;
    /// TRUE for a press, FALSE for a release
    gboolean pressed;
    /// The device the input came from, compare with get_xtest_pointer_deviceid()
    int sourceid;
} KeyState;

/**
 * Waits for the next event and decodes it into the given buffer.
 * Not every X event is an input event (keymap changes, auto-repeat, events
 * from other extensions, ...), so the return value has to be checked before
 * the buffer is used, otherwise unrelated events get mistaken for key presses.
 * @returns TRUE when a key or button state was decoded into the buffer
 */
gboolean get_next_key_state(Display *display, KeyState *buffer);

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
 * Converts a keycode or a mouse button number into a name to show the user,
 * ex. "a", "Shift_L" or "Mouse 9".
 * @returns A newly allocated string, never NULL. Free with g_free().
 */
char *input_to_string(Display *display, int code, enum InputType type);

/**
 * Device id of the XTEST virtual pointer, or -1 when there is none.
 * The clicks xclicker sends itself are reported as coming from this device,
 * so hotkey handling can tell them apart from real ones.
 */
int get_xtest_pointer_deviceid(Display *display);

/**
 * Gets a new display.
 * Use XCloseDisplay after use.
 */
Display *get_display();

#endif
