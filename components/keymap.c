/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>

#include "../util.h"

#define LAYOUT_MAX 256

/* Given a token (sym) from the xkb_symbols string
 * check whether it is a valid layout/variant. The
 * EXCLUDES array contains invalid layouts/variants
 * that are part of the xkb rules config.
 */
static int
IsLayoutOrVariant(char *sym)
{
	static const char* EXCLUDES[] = { "evdev", "inet", "pc", "base" };

	size_t i;
	for (i = 0; i < sizeof(EXCLUDES)/sizeof(EXCLUDES[0]); ++i)
		if (strstr(sym, EXCLUDES[i])) {
			return 0;
		}

	return 1;
}

static void
GetKeyLayout(char *syms, char layout[], int groupNum)
{
	char *token, *copy, *delims;
	int group;

	delims = "+:";
	group = 0;
	copy = strdup(syms);
	token = strtok(copy, delims);
	while (token != NULL && group <= groupNum) {
		/* Ignore :2,:3,:4 which represent additional layout
 		 * groups
 		 */
		if (IsLayoutOrVariant(token)
		    && !(strlen(token) == 1 && isdigit(token[0]))) {
			strncpy (layout, token, LAYOUT_MAX);
			group++;
		}

		token = strtok(NULL,delims);
	}

	free(copy);
}

const char *
keymap(void)
{
	static char layout[LAYOUT_MAX];

	Display *dpy;
	char *symbols = NULL;
	XkbDescRec* desc = NULL;

	memset(layout, '\0', LAYOUT_MAX);

	if (!(dpy = XOpenDisplay(NULL))) {
		warn("XOpenDisplay: Failed to open display");
		return NULL;
	}

	;
	if (!(desc = XkbAllocKeyboard())) {
		warn("XkbAllocKeyboard: failed to allocate keyboard");
		XCloseDisplay(dpy);
		return NULL;
	}

	XkbGetNames(dpy, XkbSymbolsNameMask, desc);
	if (desc->names) {
		XkbStateRec state;
		XkbGetState(dpy, XkbUseCoreKbd, &state);

		symbols = XGetAtomName(dpy, desc->names->symbols);
		GetKeyLayout(symbols, layout, state.group);
		XFree(symbols);
	} else {
		warn("XkbGetNames: failed to retrieve symbols for keys");
		return NULL;
	}

	XkbFreeKeyboard(desc, XkbSymbolsNameMask, 1);
	XCloseDisplay(dpy);

	return layout;
}
