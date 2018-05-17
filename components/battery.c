/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../util.h"

#if defined(__linux__)
	#include <limits.h>

	const char *
	battery_perc(const char *bat)
	{
		int perc;
		char path[PATH_MAX];

		snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/",
		         bat, "/capacity");
		return (pscanf(path, "%i", &perc) == 1) ? bprintf("%d", perc) : NULL;
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
		};
		size_t i;
		char path[PATH_MAX], state[12];

		snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/",
		         bat, "/status");
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
#elif defined(__OpenBSD__)
	#include <fcntl.h>
	#include <machine/apmvar.h>
	#include <sys/ioctl.h>
	#include <unistd.h>

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
			fprintf(stderr, "ioctl 'APM_IOC_GETPOWER': %s\n",
			        strerror(errno));
			close(fd);
			return NULL;
		}
		close(fd);

		return bprintf("%d", apm_info.battery_life);
	}

	const char *
	battery_state(const char *bat)
	{
		int fd, i;
		struct apm_power_info apm_info;
		struct {
			unsigned int state;
			char *symbol;
		} map[] = {
			{ APM_AC_ON,      "+" },
			{ APM_AC_OFF,     "-" },
		};

		fd = open("/dev/apm", O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "open '/dev/apm': %s\n", strerror(errno));
			return NULL;
		}

		if (ioctl(fd, APM_IOC_GETPOWER, &apm_info) < 0) {
			fprintf(stderr, "ioctl 'APM_IOC_GETPOWER': %s\n",
			        strerror(errno));
			close(fd);
			return NULL;
		}
		close(fd);

		for (i = 0; i < LEN(map); i++) {
			if (map[i].state == apm_info.ac_state) {
				break;
			}
		}
		return (i == LEN(map)) ? "?" : map[i].symbol;
	}
#endif
