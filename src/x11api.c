#include <gtk/gtk.h>
#include "x11api.h"

int xevent(Display *display, long mask, XEvent event)
{
    if (!XSendEvent(display, PointerWindow, True, mask, &event))
        return 0;
    XFlush(display);
    usleep(1);
    return 1;
}

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
    if (!xevent(display, ButtonPressMask, event))
        return 0;
    // Release
    event.type = ButtonRelease;
    if (!xevent(display, ButtonReleaseMask, event))
        return 0;

    return 1;
}

Display *get_display()
{
    return XOpenDisplay(NULL);
}