/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../util.h"

#if defined(__linux__)
	static size_t
	pread(const char *path, char *buf, size_t bufsiz)
	{
		FILE *fp;
		size_t bytes_read;

		if (!(fp = fopen(path, "r"))) {
			warn("fopen '%s':", path);
			return 0;
		}
		if (!(bytes_read = fread(buf, sizeof(char), bufsiz, fp))) {
			warn("fread '%s':", path);
			fclose(fp);
			return 0;
		}
		fclose(fp);

		buf[bytes_read] = '\0';

		return bytes_read;
	}

	const char *
	swap_free(void)
	{
		long total, free;
		char *match;

		if (!pread("/proc/meminfo", buf, sizeof(buf) - 1)) {
			return NULL;
		}

		if (!(match = strstr(buf, "SwapTotal"))) {
			return NULL;
		}
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		if (!(match = strstr(buf, "SwapFree"))) {
			return NULL;
		}
		sscanf(match, "SwapFree: %ld kB\n", &free);

		return fmt_human(free * 1024, 1024);
	}

	const char *
	swap_perc(void)
	{
		long total, free, cached;
		char *match;

		if (!pread("/proc/meminfo", buf, sizeof(buf) - 1)) {
			return NULL;
		}

		if (!(match = strstr(buf, "SwapTotal"))) {
			return NULL;
		}
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		if (!(match = strstr(buf, "SwapCached"))) {
			return NULL;
		}
		sscanf(match, "SwapCached: %ld kB\n", &cached);

		if (!(match = strstr(buf, "SwapFree"))) {
			return NULL;
		}
		sscanf(match, "SwapFree: %ld kB\n", &free);

		if (total == 0) {
			return NULL;
		}

		return bprintf("%d", 100 * (total - free - cached) / total);
	}

	const char *
	swap_total(void)
	{
		long total;
		char *match;

		if (!pread("/proc/meminfo", buf, sizeof(buf) - 1)) {
			return NULL;
		}

		if (!(match = strstr(buf, "SwapTotal"))) {
			return NULL;
		}
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		return fmt_human(total * 1024, 1024);
	}

	const char *
	swap_used(void)
	{
		long total, free, cached;
		char *match;

		if (!pread("/proc/meminfo", buf, sizeof(buf) - 1)) {
			return NULL;
		}

		if (!(match = strstr(buf, "SwapTotal"))) {
			return NULL;
		}
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		if (!(match = strstr(buf, "SwapCached"))) {
			return NULL;
		}
		sscanf(match, "SwapCached: %ld kB\n", &cached);

		if (!(match = strstr(buf, "SwapFree"))) {
			return NULL;
		}
		sscanf(match, "SwapFree: %ld kB\n", &free);

		return fmt_human((total - free - cached) * 1024, 1024);
	}
#elif defined(__OpenBSD__)
	#include <stdlib.h>
	#include <sys/param.h> /* dbtob */
	#include <sys/swap.h>
	#include <sys/types.h>
	#include <unistd.h>

	static int
	getstats(int *total, int *used)
	{
		struct swapent *sep, *fsep;
		int rnswap, nswap, i;

		if ((nswap = swapctl(SWAP_NSWAP, 0, 0)) < 1) {
			warn("swaptctl 'SWAP_NSWAP':");
			return 1;
		}
		if (!(fsep = sep = calloc(nswap, sizeof(*sep)))) {
			warn("calloc 'nswap':");
			return 1;
		}
		if ((rnswap = swapctl(SWAP_STATS, (void *)sep, nswap)) < 0) {
			warn("swapctl 'SWAP_STATA':");
			return 1;
		}
		if (nswap != rnswap) {
			warn("getstats: SWAP_STATS != SWAP_NSWAP");
			return 1;
		}

		*total = 0;
		*used = 0;

		for (i = 0; i < rnswap; i++) {
			*total += sep->se_nblks >> 1;
			*used += sep->se_inuse >> 1;
		}

		free(fsep);

		return 0;
	}

	const char *
	swap_free(void)
	{
		int total, used;

		if (getstats(&total, &used)) {
			return NULL;
		}

		return fmt_human((total - used) * 1024, 1024);
	}

	const char *
	swap_perc(void)
	{
		int total, used;

		if (getstats(&total, &used)) {
			return NULL;
		}

		if (total == 0) {
			return NULL;
		}

		return bprintf("%d", 100 * used / total);
	}

	const char *
	swap_total(void)
	{
		int total, used;

		if (getstats(&total, &used)) {
			return NULL;
		}

		return fmt_human(total * 1024, 1024);
	}

	const char *
	swap_used(void)
	{
		int total, used;

		if (getstats(&total, &used)) {
			return NULL;
		}

		return fmt_human(used * 1024, 1024);
	}
#endif
