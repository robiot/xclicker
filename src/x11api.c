#include <gtk/gtk.h>
#include "x11api.h"

void get_mouse_coords(Display *display, int *x, int *y)
{
    XEvent event;
    XQueryPointer(display, DefaultRootWindow(display),
                  &event.xbutton.root, &event.xbutton.window,
                  &event.xbutton.x_root, &event.xbutton.y_root,
                  &event.xbutton.x, &event.xbutton.y,
                  &event.xbutton.state);
    *x = event.xbutton.x;
    *y = event.xbutton.y;
}

// Mode:
// 1: Only mouse
// 2: Only keyboard keys
void mask_config(Display *display, int mode)
{
    XIEventMask mask[2];
    XIEventMask *m;
    Window win = DefaultRootWindow(display);

    m = &mask[0];
    m->deviceid = XIAllDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));

    if (mode == 1)
        XISetMask(m->mask, XI_ButtonPress);
    else if (mode == 2)
        XISetMask(m->mask, XI_KeyPress);

    m = &mask[1];
    m->deviceid = XIAllMasterDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = calloc(m->mask_len, sizeof(char));
    if (mode == 1)
        XISetMask(m->mask, XI_RawButtonPress);
    //XISetMask(m->mask, XI_RawKeyPress); // Not needed, makes input doubled

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

    // while (1)
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

int cxevent(Display *display, long mask, XEvent event)
{
    if (!XSendEvent(display, PointerWindow, True, mask, &event))
        return 0;
    XFlush(display);
    usleep(1);
    return 1;
}

// Click on current mouse position with given button
int click(Display *display, int button)
{
    // Create event
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    event.xbutton.subwindow = DefaultRootWindow(display);
    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window,
                      &event.xbutton.root, &event.xbutton.subwindow,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);
    }

    // Press
    event.type = ButtonPress;
    if (!cxevent(display, ButtonPressMask, event))
        return 0;
    // Release
    event.type = ButtonRelease;
    if (!cxevent(display, ButtonReleaseMask, event))
        return 0;

    return 1;
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