/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <limits.h>

#include "../util.h"

#if defined(__linux__)
	const char *
	netspeed_rx(const char *interface)
	{
		static int valid;
		static unsigned long long rxbytes;
		unsigned long oldrxbytes;
		extern const unsigned int interval;
		char path[PATH_MAX];

		oldrxbytes = rxbytes;
		snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", interface);
		if (pscanf(path, "%llu", &rxbytes) != 1) {
			return NULL;
		}
		if (!valid) {
			valid = 1;
			return NULL;
		}

		return fmt_scaled((rxbytes - oldrxbytes) / interval * 1000);
	}

	const char *
	netspeed_tx(const char *interface)
	{
		static int valid;
		static unsigned long long txbytes;
		unsigned long oldtxbytes;
		extern const unsigned int interval;
		char path[PATH_MAX];

		oldtxbytes = txbytes;
		snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", interface);
		if (pscanf(path, "%llu", &txbytes) != 1) {
			return NULL;
		}
		if (!valid) {
			valid = 1;
			return NULL;
		}

		return fmt_scaled((txbytes - oldtxbytes) / interval * 1000);
	}
#elif defined(__OpenBSD__)
	/* unimplemented */
#endif
