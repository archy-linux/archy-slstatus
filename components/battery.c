/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#if defined(__linux__)
#include <limits.h>
#include <string.h>
#elif defined(__OpenBSD__)
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <machine/apmvar.h>
#endif

#include "../util.h"

const char *
battery_perc(const char *bat)
{
#if defined(__linux__)
	int perc;
	char path[PATH_MAX];

	snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/", bat, "/capacity");
	return (pscanf(path, "%i", &perc) == 1) ?
	       bprintf("%d", perc) : NULL;
#elif defined(__OpenBSD__)
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
#endif
}

#if defined(__linux__)
const char *
battery_power(const char *bat)
{
	int watts;
	char path[PATH_MAX];

	snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/", bat, "/power_now");
	return (pscanf(path, "%i", &watts) == 1) ?
	       bprintf("%d", (watts + 500000) / 1000000) : NULL;
}

const char *
battery_state(const char *bat)
{
	struct {
		char *state;
		char *symbol;
	} map[] = {
		{ "Charging",    "+" },
		{ "Discharging", "-" },
		{ "Full",        "=" },
		{ "Unknown",     "/" },
	};
	size_t i;
	char path[PATH_MAX], state[12];

	snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/", bat, "/status");
	if (pscanf(path, "%12s", state) != 1) {
		return NULL;
	}

	for (i = 0; i < LEN(map); i++) {
		if (!strcmp(map[i].state, state)) {
			break;
		}
	}
	return (i == LEN(map)) ? "?" : map[i].symbol;
}
#endif
