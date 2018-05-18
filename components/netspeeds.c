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
	#include <string.h>
	#include <ifaddrs.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <net/if.h>

	const char *
	netspeed_rx(const char *interface)
	{
		struct ifaddrs *ifal, *ifa;
		struct if_data *ifd;
		static uint64_t oldrxbytes;
		uint64_t rxbytes = 0;
		const char *rxs;
		extern const unsigned int interval;

		if (getifaddrs(&ifal) == -1) {
			warn("getifaddrs failed");
			return NULL;
		}
		for (ifa = ifal; ifa; ifa = ifa->ifa_next) {
			if (!strcmp(ifa->ifa_name, interface) &&
			   (ifd = (struct if_data *)ifa->ifa_data)) {
				rxbytes += ifd->ifi_ibytes;
			}
		}
		freeifaddrs(ifal);

		rxs = oldrxbytes ? fmt_scaled((rxbytes - oldrxbytes) /
		                              interval * 1000) : NULL;
		return (oldrxbytes = rxbytes, rxs);
	}

	const char *
	netspeed_tx(const char *interface)
	{
		struct ifaddrs *ifal, *ifa;
		struct if_data *ifd;
		static uint64_t oldtxbytes;
		uint64_t txbytes = 0;
		const char *txs;
		extern const unsigned int interval;

		if (getifaddrs(&ifal) == -1) {
			warn("getifaddrs failed");
			return NULL;
		}
		for (ifa = ifal; ifa; ifa = ifa->ifa_next) {
			if (!strcmp(ifa->ifa_name, interface) &&
			   (ifd = (struct if_data *)ifa->ifa_data)) {
				txbytes += ifd->ifi_obytes;
			}
		}
		freeifaddrs(ifal);

		txs = oldtxbytes ? fmt_scaled((txbytes - oldtxbytes) /
		                              interval * 1000) : NULL;
		return (oldtxbytes = txbytes, txs);
	}
#endif
