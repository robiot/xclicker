#ifndef __X11_H
#define __X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

void move_to(Display *display, int x, int y);
int click(Display *display, int button);
Display *get_display();

#endif