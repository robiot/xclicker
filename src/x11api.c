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

    if (flags & MASK_MOUSE_PRESS)
    {
        XISetMask(m->mask, XI_ButtonPress);
        XISetMask(m->mask, XI_RawButtonPress);
    }

    m = &mask[1];
    m->deviceid = XIAllMasterDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));

    XISelectEvents(display, win, &mask[0], 2);
    XSync(display, FALSE);

    free(mask[0].mask);
    free(mask[1].mask);
}

void get_next_key_state(Display *display, KeyState *buffer)
{
    // Initialize with default values
    buffer->button = 0;
    buffer->evtype = -1;

    XEvent event;

    XNextEvent(display, &event);

    XGenericEventCookie *cookie = &event.xcookie;

    if (XGetEventData(display, cookie) && cookie->type == GenericEvent)
    {
        XIDeviceEvent *event = cookie->data;

        if (!(event->flags & XIKeyRepeat)) {
            buffer->evtype = event->evtype;
            buffer->button = event->detail;
        }
    }

    XFreeEventData(display, cookie);
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

char *keycode_to_string(Display *display, int keycode)
{
    return XKeysymToString(XkbKeycodeToKeysym(display, keycode, 0, 0));
}

Display *get_display()
{
    return XOpenDisplay(NULL);
}
