/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <X11/Xlib.h>

#include "../util.h"

const char *
keyboard_indicators(void)
{
	Display *dpy = XOpenDisplay(NULL);
	XKeyboardState state;

	if (dpy == NULL) {
		fprintf(stderr, "Cannot open display\n");
		return NULL;
	}
	XGetKeyboardControl(dpy, &state);
	XCloseDisplay(dpy);

	switch (state.led_mask) {
		case 1:
			return "c";
		case 2:
			return "n";
		case 3:
			return "cn";
		default:
			return "";
	}
}
