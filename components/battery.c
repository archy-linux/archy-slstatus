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
		return (pscanf(path, "%d", &perc) == 1) ? bprintf("%d", perc) : NULL;
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

	const char *
	battery_remaining(const char *bat)
	{
		int charge_now, current_now, m, h;
		float timeleft;
		char path[PATH_MAX], state[12];

		snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/",
		         bat, "/status");
		if (pscanf(path, "%12s", state) != 1) {
			return NULL;
		}

		if (!strcmp(state, "Discharging")) {
			snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/",
					 bat, "/charge_now");
			if (pscanf(path, "%d", &charge_now) != 1) {
				return NULL;
			}
			snprintf(path, sizeof(path), "%s%s%s", "/sys/class/power_supply/",
					 bat, "/current_now");
			if (pscanf(path, "%d", &current_now) != 1) {
				return NULL;
			}

			timeleft = (float)charge_now / (float)current_now;
			h = timeleft;
			m = (timeleft - (float)h) * 60;

			return bprintf("%dh %dm", h, m);
		}

		return "";
	}
#elif defined(__OpenBSD__)
	#include <fcntl.h>
	#include <machine/apmvar.h>
	#include <sys/ioctl.h>
	#include <unistd.h>

	static int
	load_apm_power_info(struct apm_power_info *apm_info)
	{
		int fd;

		fd = open("/dev/apm", O_RDONLY);
		if (fd < 0) {
			warn("open '/dev/apm':");
			return 0;
		}

		memset(apm_info, 0, sizeof(struct apm_power_info));
		if (ioctl(fd, APM_IOC_GETPOWER, apm_info) < 0) {
			warn("ioctl 'APM_IOC_GETPOWER':");
			close(fd);
			return 0;
		}
		return close(fd), 1;
	}

	const char *
	battery_perc(const char *unused)
	{
		struct apm_power_info apm_info;

		if (load_apm_power_info(&apm_info)) {
			return bprintf("%d", apm_info.battery_life);
		}

		return NULL;
	}

	const char *
	battery_state(const char *unused)
	{
		struct apm_power_info apm_info;
		size_t i;
		struct {
			unsigned int state;
			char *symbol;
		} map[] = {
			{ APM_AC_ON,      "+" },
			{ APM_AC_OFF,     "-" },
		};

		if (load_apm_power_info(&apm_info)) {
			for (i = 0; i < LEN(map); i++) {
				if (map[i].state == apm_info.ac_state) {
					break;
				}
			}
			return (i == LEN(map)) ? "?" : map[i].symbol;
		}

		return NULL;
	}

	const char *
	battery_remaining(const char *unused)
	{
		struct apm_power_info apm_info;

		if (load_apm_power_info(&apm_info)) {
			if (apm_info.ac_state != APM_AC_ON) {
				return bprintf("%uh %02um", apm_info.minutes_left / 60,
				               apm_info.minutes_left % 60);
			} else {
				return "";
			}
		}

		return NULL;
	}
#endif
