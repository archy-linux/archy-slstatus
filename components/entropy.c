/* See LICENSE file for copyright and license details. */
#if defined(__linux__)
	#include <inttypes.h>
	#include <stdio.h>

	#include "../util.h"

	const char *
	entropy(void)
	{
		uintmax_t num;

		if (pscanf("/proc/sys/kernel/random/entropy_avail",
		           "%" PRIuMAX, &num) != 1) {
			return NULL;
		}

		return bprintf("%" PRIuMAX, num);
	}
#elif defined(__OpenBSD__)
	const char *
	entropy(void)
	{
		/* Unicode Character 'INFINITY' (U+221E) */
		return "\xe2\x88\x9e";
	}
#endif
