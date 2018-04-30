/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>

#include "../../util.h"

const char *
uptime(void)
{
	int h;
	int m;
	int uptime = 0;
	struct sysinfo info;

	sysinfo(&info);
	uptime = info.uptime;

	h = uptime / 3600;
	m = (uptime - h * 3600) / 60;

	return bprintf("%dh %dm", h, m);
}
