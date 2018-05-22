/* See LICENSE file for copyright and license details. */
#include "../util.h"

#if defined(__linux__)
	#include <limits.h>

	#define BRIGHTNESS_MAX "/sys/class/backlight/%s/max_brightness"
	#define BRIGHTNESS_CUR "/sys/class/backlight/%s/brightness"

	const char *
	backlight_perc(const char *card)
	{
		char path[PATH_MAX];
		int max, cur;

		if (esnprintf(path, sizeof (path), BRIGHTNESS_MAX, card) < 0 ||
			pscanf(path, "%d", &max) != 1) {
			return NULL;
		}

		if (esnprintf(path, sizeof (path), BRIGHTNESS_CUR, card) < 0 ||
			pscanf(path, "%d", &cur) != 1) {
			return NULL;
		}

		if (max == 0) {
			return NULL;
		}

		return bprintf("%d", cur * 100 / max);
	}
#endif
