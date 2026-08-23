#include <gtk/gtk.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include "x11api.h"

#define DEFAULT_MICRO_SLEEP 1

void mask_config(Display *display, enum MaskFlags flags)
{
    XIEventMask mask[2];
    XIEventMask *m;
    Window win = DefaultRootWindow(display);

    m = &mask[0];
    m->deviceid = XIAllDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));

    if (flags & MASK_KEYBOARD_PRESS)
        XISetMask(m->mask, XI_KeyPress);

    if (flags & MASK_KEYBOARD_RELEASE)
        XISetMask(m->mask, XI_KeyRelease);

    m = &mask[1];
    m->deviceid = XIAllMasterDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));

    /*
     * Buttons use raw events because XI_ButtonPress on the root window is only
     * delivered when the click is not consumed by the window under the cursor,
     * which is never the case for a global hotkey. Raw events always arrive.
     *
     * They are selected on the master devices only. Selecting them on
     * XIAllDevices delivers every press twice (once per matching device),
     * which would toggle clicking on and straight back off again.
     */
    if (flags & MASK_MOUSE_PRESS)
        XISetMask(m->mask, XI_RawButtonPress);

    if (flags & MASK_MOUSE_RELEASE)
        XISetMask(m->mask, XI_RawButtonRelease);

    XISelectEvents(display, win, &mask[0], 2);
    XSync(display, FALSE);

    free(mask[0].mask);
    free(mask[1].mask);
}

gboolean get_next_key_state(Display *display, KeyState *buffer)
{
    // Initialize with default values
    buffer->button = 0;
    buffer->evtype = -1;
    buffer->type = INPUT_TYPE_KEY;
    buffer->pressed = FALSE;
    buffer->sourceid = -1;

    XEvent event;

    XNextEvent(display, &event);

    // Sent unconditionally to every client, so it turns up here too.
    // The cached keyboard mapping has to be refreshed or keysym lookups go stale.
    if (event.type == MappingNotify)
    {
        XRefreshKeyboardMapping(&event.xmapping);
        return FALSE;
    }

    // Only generic events carry XInput2 data, anything else has no cookie to read
    if (event.type != GenericEvent)
        return FALSE;

    XGenericEventCookie *cookie = &event.xcookie;
    gboolean has_state = FALSE;

    if (XGetEventData(display, cookie))
    {
        switch (cookie->evtype)
        {
        case XI_KeyPress:
        case XI_KeyRelease:
        {
            XIDeviceEvent *device_event = cookie->data;

            // Auto-repeat is not a new key press
            if (!(device_event->flags & XIKeyRepeat))
            {
                buffer->evtype = device_event->evtype;
                buffer->button = device_event->detail;
                buffer->type = INPUT_TYPE_KEY;
                buffer->pressed = (device_event->evtype == XI_KeyPress);
                buffer->sourceid = device_event->sourceid;
                has_state = TRUE;
            }
            break;
        }
        case XI_RawButtonPress:
        case XI_RawButtonRelease:
        {
            /*
             * Raw events are XIRawEvent, which stops matching XIDeviceEvent
             * after `detail`, so reading `flags` off the wrong struct would
             * pick up an unrelated field.
             */
            XIRawEvent *raw_event = cookie->data;

            // Buttons emulated from a smooth scroll axis duplicate the axis
            // event, they are not something the user pressed.
            if (!(raw_event->flags & XIPointerEmulated))
            {
                buffer->evtype = raw_event->evtype;
                buffer->button = raw_event->detail;
                buffer->type = INPUT_TYPE_BUTTON;
                buffer->pressed = (raw_event->evtype == XI_RawButtonPress);
                buffer->sourceid = raw_event->sourceid;
                has_state = TRUE;
            }
            break;
        }
        }
    }

    XFreeEventData(display, cookie);

    return has_state;
}

void get_cursor_coords(Display *display, int *x, int *y)
{
    XButtonEvent event;
    XQueryPointer(display, DefaultRootWindow(display),
                  &event.root, &event.window,
                  &event.x_root, &event.y_root,
                  &event.x, &event.y,
                  &event.state);
    *x = event.x;
    *y = event.y;
}

void move_to(Display *display, int x, int y)
{
    int cur_x, cur_y;
    get_cursor_coords(display, &cur_x, &cur_y);
    XWarpPointer(display, None, None, 0, 0, 0, 0, -cur_x, -cur_y); // For absolute position
    XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
    usleep(DEFAULT_MICRO_SLEEP);
}

int mouse_event(Display *display, int button, int mode, enum MouseEvents event_type)
{
    switch (mode)
    {
    case CLICK_MODE_XEVENT:
    {
        XButtonEvent event;
        memset(&event, 0, sizeof(event));
        event.button = button;
        event.same_screen = True;
        event.subwindow = DefaultRootWindow(display);

        while (event.subwindow)
        {
            event.window = event.subwindow;
            XQueryPointer(display, event.window,
                          &event.root, &event.subwindow,
                          &event.x_root, &event.y_root,
                          &event.x, &event.y,
                          &event.state);
        }

        // Press
        event.type = (event_type == MOUSE_EVENT_PRESS) ? ButtonPress : ButtonRelease;

        if (!XSendEvent(display, PointerWindow, True, ButtonPressMask, (XEvent *)&event))
            return FALSE;
        XFlush(display);
        usleep(DEFAULT_MICRO_SLEEP);
        break;
    }
    case CLICK_MODE_XTEST:
        XTestFakeButtonEvent(display, button, (event_type == MOUSE_EVENT_PRESS), CurrentTime);
        XFlush(display);
        break;
    }
    return TRUE;
}

int click(Display *display, int button, int mode, int sleep)
{
    if (!mouse_event(display, button, mode, MOUSE_EVENT_PRESS))
        return FALSE;

    if (sleep != 0)
        usleep(sleep);

    return mouse_event(display, button, mode, MOUSE_EVENT_RELEASE);
}

char *input_to_string(Display *display, int code, enum InputType type)
{
    if (type == INPUT_TYPE_BUTTON)
    {
        switch (code)
        {
        case Button1:
            return g_strdup("Mouse Left");
        case Button2:
            return g_strdup("Mouse Middle");
        case Button3:
            return g_strdup("Mouse Right");
        case Button4:
            return g_strdup("Scroll Up");
        case Button5:
            return g_strdup("Scroll Down");
        default:
            return g_strdup_printf("Mouse %d", code);
        }
    }

    const char *keysym_name = XKeysymToString(XkbKeycodeToKeysym(display, code, 0, 0));

    /*
     * A keycode that is unused in the current layout has no keysym, and
     * XKeysymToString then returns NULL. Callers build labels out of this, so
     * give the key a name of its own instead of handing out a NULL.
     */
    if (keysym_name == NULL)
        return g_strdup_printf("Key %d", code);

    return g_strdup(keysym_name);
}

int get_xtest_pointer_deviceid(Display *display)
{
    int device_count = 0;
    int deviceid = -1;
    XIDeviceInfo *devices = XIQueryDevice(display, XIAllDevices, &device_count);

    if (devices == NULL)
        return -1;

    for (int i = 0; i < device_count; i++)
    {
        if (devices[i].use != XISlavePointer || devices[i].name == NULL)
            continue;

        if (strstr(devices[i].name, "XTEST") != NULL)
        {
            deviceid = devices[i].deviceid;
            break;
        }
    }

    XIFreeDeviceInfo(devices);

    return deviceid;
}

Display *get_display()
{
    return XOpenDisplay(NULL);
}
