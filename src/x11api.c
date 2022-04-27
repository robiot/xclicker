#include <gtk/gtk.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include "x11api.h"

#define DEFAULT_MICRO_SLEEP 1

void mask_config(Display *display, int mode)
{
    XIEventMask mask[2];
    XIEventMask *m;
    Window win = DefaultRootWindow(display);

    m = &mask[0];
    m->deviceid = XIAllDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));

    if (mode == MASK_CONFIG_MOUSE)
        XISetMask(m->mask, XI_ButtonPress);
    else if (mode == MASK_CONFIG_KEYBOARD)
        XISetMask(m->mask, XI_KeyPress);

    m = &mask[1];
    m->deviceid = XIAllMasterDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));
    if (mode == MASK_CONFIG_MOUSE)
        XISetMask(m->mask, XI_RawButtonPress);

    XISelectEvents(display, win, &mask[0], 2);
    XSync(display, FALSE);

    free(mask[0].mask);
    free(mask[1].mask);
}

int get_next_key_state(Display *display)
{
    int button = 0;
    XEvent event;
    XGenericEventCookie *cookie = (XGenericEventCookie *)&event.xcookie;
    XNextEvent(display, (XEvent *)&event);

    if (XGetEventData(display, cookie) && cookie->type == GenericEvent)
    {
        XIDeviceEvent *event = cookie->data;
        if (!(event->flags & XIKeyRepeat))
            button = event->detail;
    }

    XFreeEventData(display, cookie);
    return button;
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

/**
 * Custom Xevent.
 * Does everything needed for xsendevent.
 */
int cxevent(Display *display, long mask, XButtonEvent event)
{
    if (!XSendEvent(display, PointerWindow, True, mask, (XEvent *)&event))
        return FALSE;
    XFlush(display);
    usleep(DEFAULT_MICRO_SLEEP);
    return TRUE;
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
        return cxevent(display, ButtonPressMask, event);
    }
    case CLICK_MODE_XTEST:
        XTestFakeButtonEvent(display, button, (event_type == MOUSE_EVENT_PRESS), CurrentTime);
        XFlush(display);
        break;
    }
    return TRUE;
}

int click(Display *display, int button, int mode)
{
    if (!mouse_event(display, button, mode, MOUSE_EVENT_PRESS))
        return FALSE;
    if (mode == CLICK_MODE_XTEST)
        usleep(DEFAULT_MICRO_SLEEP);
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
