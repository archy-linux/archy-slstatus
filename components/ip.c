/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#if defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "../util.h"

const char *
ipv4(const char *iface)
{
	struct ifaddrs *ifaddr, *ifa;
	int s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) < 0) {
		fprintf(stderr, "getifaddrs: %s\n", strerror(errno));
		return NULL;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
		                NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if (!strcmp(ifa->ifa_name, iface) &&
		    (ifa->ifa_addr->sa_family == AF_INET)) {
			if (s != 0) {
				fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
				return NULL;
			}
			return bprintf("%s", host);
		}
	}

	freeifaddrs(ifaddr);

	return NULL;
}

const char *
ipv6(const char *iface)
{
	struct ifaddrs *ifaddr, *ifa;
	int s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) < 0) {
		fprintf(stderr, "getifaddrs: %s\n", strerror(errno));
		return NULL;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), host,
		                NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if (!strcmp(ifa->ifa_name, iface) &&
		    (ifa->ifa_addr->sa_family == AF_INET6)) {
			if (s != 0) {
				fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
				return NULL;
			}
			return bprintf("%s", host);
		}
	}

	freeifaddrs(ifaddr);

	return NULL;
}
