#include <gtk/gtk.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include "x11api.h"

void get_mouse_coords(Display *display, int *x, int *y)
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

// Configure XInput masks for given display
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

int get_event_button_id(XIDeviceEvent *event)
{
    return event->detail;
}

int get_button_state(Display *display)
{
    int button = 0;
    XEvent event;
    XGenericEventCookie *cookie = (XGenericEventCookie *)&event.xcookie;
    XNextEvent(display, (XEvent *)&event);

    if (XGetEventData(display, cookie) && cookie->type == GenericEvent)
        button = get_event_button_id(cookie->data);

    XFreeEventData(display, cookie);
    return button;
}

// Move mouse pointer to given coords
void move_to(Display *display, int x, int y)
{
    int cur_x, cur_y;
    get_mouse_coords(display, &cur_x, &cur_y);
    XWarpPointer(display, None, None, 0, 0, 0, 0, -cur_x, -cur_y); // For absolute position
    XWarpPointer(display, None, None, 0, 0, 0, 0, x, y);
    usleep(1);
}

int cxevent(Display *display, long mask, XButtonEvent event)
{
    if (!XSendEvent(display, PointerWindow, True, mask, (XEvent *)&event))
        return FALSE;
    XFlush(display);
    usleep(1);
    return TRUE;
}

// Click on current mouse position with given button
int click(Display *display, int button, int mode)
{
    switch (mode)
    {
    case CLICK_MODE_XEVENT:
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
        event.type = ButtonPress;
        if (!cxevent(display, ButtonPressMask, event))
            return FALSE;

        // Release
        event.type = ButtonRelease;
        if (!cxevent(display, ButtonReleaseMask, event))
            return FALSE;
        break;

    case CLICK_MODE_XTEST:
        XTestFakeButtonEvent(display, button, True, CurrentTime);
        XFlush(display);
        usleep(1);
        XTestFakeButtonEvent(display, button, False, CurrentTime);
        XFlush(display);
    }

    return TRUE;
}

// https://stackoverflow.com/questions/9838385/replace-of-xkeycodetokeysym
// https://linux.die.net/man/3/xkbkeycodetokeysym
char *keycode_to_string(Display *display, int keycode)
{
    return XKeysymToString(XkbKeycodeToKeysym(display, keycode, 0, 0));
}

Display *get_display()
{
    return XOpenDisplay(NULL);
}