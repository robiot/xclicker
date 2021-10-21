#ifndef __X11_H
#define __X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

void mask_config(Display *display, int mode);
int get_button_state(Display *display);

void get_mouse_coords(Display *display, int *x, int *y);
void move_to(Display *display, int x, int y);
int click(Display *display, int button);

char *keycode_to_string(Display *display, int keycode);
Display *get_display();

#endif