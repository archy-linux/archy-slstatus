/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../util.h"

#if defined(__linux__)
	#include <limits.h>
	#include <linux/wireless.h>

	const char *
	wifi_perc(const char *iface)
	{
		int i, cur;
		int total = 70; /* the max of /proc/net/wireless */
		char *p, *datastart;
		char path[PATH_MAX];
		char status[5];
		FILE *fp;

		snprintf(path, sizeof(path), "%s%s%s", "/sys/class/net/", iface,
		         "/operstate");
		if (!(fp = fopen(path, "r"))) {
			fprintf(stderr, "fopen '%s': %s\n", path,
			        strerror(errno));
			return NULL;
		}
		p = fgets(status, 5, fp);
		fclose(fp);
		if(!p || strcmp(status, "up\n") != 0) {
			return NULL;
		}

		if (!(fp = fopen("/proc/net/wireless", "r"))) {
			fprintf(stderr, "fopen '/proc/net/wireless': %s\n",
			        strerror(errno));
			return NULL;
		}

		for (i = 0; i < 3; i++) {
			if (!(p = fgets(buf, sizeof(buf) - 1, fp)))
				break;
		}
		fclose(fp);
		if (i < 2 || !p) {
			return NULL;
		}

		if (!(datastart = strstr(buf, iface))) {
			return NULL;
		}

		datastart = (datastart+(strlen(iface)+1));
		sscanf(datastart + 1, " %*d   %d  %*d  %*d\t\t  %*d\t   "
		       "%*d\t\t%*d\t\t %*d\t  %*d\t\t %*d", &cur);

		return bprintf("%d", (int)((float)cur / total * 100));
	}

	const char *
	wifi_essid(const char *iface)
	{
		static char id[IW_ESSID_MAX_SIZE+1];
		int sockfd;
		struct iwreq wreq;

		memset(&wreq, 0, sizeof(struct iwreq));
		wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;
		snprintf(wreq.ifr_name, sizeof(wreq.ifr_name), "%s", iface);

		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			fprintf(stderr, "socket 'AF_INET': %s\n",
			        strerror(errno));
			return NULL;
		}
		wreq.u.essid.pointer = id;
		if (ioctl(sockfd,SIOCGIWESSID, &wreq) < 0) {
			fprintf(stderr, "ioctl 'SIOCGIWESSID': %s\n", strerror(errno));
			close(sockfd);
			return NULL;
		}

		close(sockfd);

		if (!strcmp(id, "")) {
			return NULL;
		}

		return id;
	}
#elif defined(__OpenBSD__)
	#include <net/if.h>
	#include <net/if_media.h>
	#include <net80211/ieee80211.h>
	#include <net80211/ieee80211_ioctl.h>
	#include <stdlib.h>
	#include <sys/types.h>

	static int
	load_ieee80211_nodereq(const char *iface, struct ieee80211_nodereq *nr)
	{
		struct ieee80211_bssid bssid;
		int sockfd;

		memset(&bssid, 0, sizeof(bssid));
		memset(nr, 0, sizeof(struct ieee80211_nodereq));
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			fprintf(stderr, "socket 'AF_INET': %s\n",
				strerror(errno));
			return 0;
		}
		strlcpy(bssid.i_name, iface, sizeof(bssid.i_name));
		if ((ioctl(sockfd, SIOCG80211BSSID, &bssid)) < 0) {
			fprintf(stderr, "ioctl 'SIOCG80211BSSID': %s\n",
				strerror(errno));
			close(sockfd);
			return 0;
		}
		strlcpy(nr->nr_ifname, iface, sizeof(nr->nr_ifname));
		memmove(&nr->nr_macaddr, bssid.i_bssid, sizeof(nr->nr_macaddr));
		if ((ioctl(sockfd, SIOCG80211NODE, nr)) < 0 && nr->nr_rssi) {
			fprintf(stderr, "ioctl 'SIOCG80211NODE': %s\n",
				strerror(errno));
			close(sockfd);
			return 0;
		}

		return close(sockfd), 1;
	}

	const char *
	wifi_perc(const char *iface)
	{
		struct ieee80211_nodereq nr;
		int q;

		if (load_ieee80211_nodereq(iface, &nr)) {
			if (nr.nr_max_rssi) {
				q = IEEE80211_NODEREQ_RSSI(&nr);
			} else {
				q = nr.nr_rssi >= -50 ? 100 : (nr.nr_rssi <= -100 ? 0 :
				(2 * (nr.nr_rssi + 100)));
			}
			return bprintf("%d", q);
		}

		return NULL;
	}

	const char *
	wifi_essid(const char *iface)
	{
		struct ieee80211_nodereq nr;

		if (load_ieee80211_nodereq(iface, &nr)) {
			return bprintf("%s", nr.nr_nwid);
		}

		return NULL;
	}
#endif
