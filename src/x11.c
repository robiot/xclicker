#include <gtk/gtk.h>
#include "x11.h"

int xevent(Display *display, XEvent event) {
    if (XSendEvent(display, PointerWindow, True, ButtonPressMask, &event) == 0)
        return -1;
    return 0;
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
    if (!xevent(display, event))
        return -1;
    usleep(1);

    // Release
    event.type = ButtonRelease;
    if (!xevent(display, event))
        return -1;
    usleep(1);

    XFlush(display);
    return 0;
}

Display *get_display()
{
    return XOpenDisplay(NULL);
}