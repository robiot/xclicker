#ifndef __X11_H
#define __X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define MASK_CONFIG_MOUSE (0)
#define MASK_CONFIG_KEYBOARD (1)

#define CLICK_MODE_XTEST (0) // This is default
#define CLICK_MODE_XEVENT (1)

void mask_config(Display *display, int mode);
int get_button_state(Display *display);

void get_mouse_coords(Display *display, int *x, int *y);
void move_to(Display *display, int x, int y);
int click(Display *display, int button, int mode);

char *keycode_to_string(Display *display, int keycode);
Display *get_display();

#endif