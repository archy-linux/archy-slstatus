/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysctl.h>

#include "../../util.h"

const char *
cpu_freq(void)
{
	int freq, mib[2];
	size_t size;

	mib[0] = CTL_HW;
	mib[1] = HW_CPUSPEED;

	size = sizeof(freq);

	if (sysctl(mib, 2, &freq, &size, NULL, 0) == -1) {
		fprintf(stderr, "sysctl 'HW_CPUSPEED': %s\n", strerror(errno));
		return NULL;
	}

	return bprintf("%d", freq);
}
