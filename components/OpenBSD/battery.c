/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <machine/apmvar.h>

#include "../../util.h"

const char *
battery_perc(const char *null)
{
	struct apm_power_info apm_info;
	int fd;

	fd = open("/dev/apm", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open '/dev/apm': %s\n", strerror(errno));
		return NULL;
	}

	if (ioctl(fd, APM_IOC_GETPOWER, &apm_info) < 0) {
		fprintf(stderr, "ioctl 'APM_IOC_GETPOWER': %s\n", strerror(errno));
		close(fd);
		return NULL;
	}
	close(fd);

	return bprintf("%d", apm_info.battery_life);
}
