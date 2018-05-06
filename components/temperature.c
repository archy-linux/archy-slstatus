/* See LICENSE file for copyright and license details. */
#include <stddef.h>

#include "../util.h"

#if defined(__linux__)
	const char *
	temp(const char *file)
	{
		int temp;

		return (pscanf(file, "%d", &temp) == 1) ?
		       bprintf("%d", temp / 1000) : NULL;
	}
#elif defined(__OpenBSD__)
	#include <errno.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/sysctl.h>
	#include <sys/time.h>
	#include <sys/sensors.h>

	const char *
	temp(const char *null)
	{
		int mib[5];
		size_t size;
		struct sensor temp;

		mib[0] = CTL_HW;
		mib[1] = HW_SENSORS;
		mib[2] = 0; /* cpu0 */
		mib[3] = SENSOR_TEMP;
		mib[4] = 0; /* temp0 */

		size = sizeof(temp);

		if (sysctl(mib, 5, &temp, &size, NULL, 0) < 0) {
			fprintf(stderr, "sysctl 'SENSOR_TEMP': %s\n",
			        strerror(errno));
			return NULL;
		}

		/* kelvin to celsius */
		return bprintf("%d", (temp.value - 273150000) / 1000000);
	}
#endif
