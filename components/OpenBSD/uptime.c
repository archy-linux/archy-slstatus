/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/time.h>

#include "../../util.h"

const char *
uptime(void)
{
	int h;
	int m;
	int uptime = 0;

	int mib[2];
	size_t size;
	time_t now;
	struct timeval boottime;

	time(&now);

	mib[0] = CTL_KERN;
	mib[1] = KERN_BOOTTIME;

	size = sizeof(boottime);

	if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1)
		uptime = now - boottime.tv_sec;
	else {
		fprintf(stderr, "sysctl 'KERN_BOOTTIME': %s\n", strerror(errno));
		return NULL;
	}

	h = uptime / 3600;
	m = (uptime - h * 3600) / 60;

	return bprintf("%dh %dm", h, m);
}
