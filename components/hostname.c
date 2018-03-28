/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <unistd.h>

#include "../util.h"

const char *
hostname(void)
{
	if (gethostname(buf, sizeof(buf)) == -1) {
		fprintf(stderr, "gethostbyname failed");
		return NULL;
	}

	return buf;
}
