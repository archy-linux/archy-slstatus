/* See LICENSE file for copyright and license details. */
#include <stdio.h>

#include "../util.h"

static const char *
format(int uptime)
{
	int h, m;

	h = uptime / 3600;
	m = (uptime - h * 3600) / 60;

	return bprintf("%dh %dm", h, m);
}

#if defined(__linux__)
	#include <sys/sysinfo.h>

	const char *
	uptime(void)
	{
		int uptime;
		struct sysinfo info;

		sysinfo(&info);
		uptime = info.uptime;

		return format(uptime);
	}
#elif defined(__OpenBSD__)
	#include <errno.h>
	#include <string.h>
	#include <sys/sysctl.h>
	#include <sys/time.h>

	const char *
	uptime(void)
	{
		int mib[2], uptime;
		size_t size;
		time_t now;
		struct timeval boottime;

		time(&now);

		mib[0] = CTL_KERN;
		mib[1] = KERN_BOOTTIME;

		size = sizeof(boottime);

		if (sysctl(mib, 2, &boottime, &size, NULL, 0) < 0) {
			warn("sysctl 'KERN_BOOTTIME':");
			return NULL;
		}

		uptime = now - boottime.tv_sec;

		return format(uptime);
	}
#endif
