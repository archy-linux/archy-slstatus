/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <time.h>

#include "../util.h"

const char *
datetime(const char *fmt)
{
	time_t t;

	t = time(NULL);
	if (!strftime(buf, sizeof(buf), fmt, localtime(&t))) {
		fprintf(stderr, "strftime: Result string exceeds buffer size\n");
		return NULL;
	}

	return buf;
}
