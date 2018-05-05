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
			fprintf(stderr, "fopen '%s': %s\n", path,
			        strerror(errno));
			return 0;
		}
		if ((bytes_read = fread(buf, sizeof(char), bufsiz, fp)) == 0) {
			fprintf(stderr, "fread '%s': %s\n", path,
			        strerror(errno));
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

		if ((match = strstr(buf, "SwapTotal")) == NULL)
			return NULL;
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		if ((match = strstr(buf, "SwapFree")) == NULL)
			return NULL;
		sscanf(match, "SwapFree: %ld kB\n", &free);

		return bprintf("%f", (float)free / 1024 / 1024);
	}

	const char *
	swap_perc(void)
	{
		long total, free, cached;
		char *match;

		if (!pread("/proc/meminfo", buf, sizeof(buf) - 1)) {
			return NULL;
		}

		if ((match = strstr(buf, "SwapTotal")) == NULL)
			return NULL;
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		if ((match = strstr(buf, "SwapCached")) == NULL)
			return NULL;
		sscanf(match, "SwapCached: %ld kB\n", &cached);

		if ((match = strstr(buf, "SwapFree")) == NULL)
			return NULL;
		sscanf(match, "SwapFree: %ld kB\n", &free);

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

		if ((match = strstr(buf, "SwapTotal")) == NULL)
			return NULL;
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		return bprintf("%f", (float)total / 1024 / 1024);
	}

	const char *
	swap_used(void)
	{
		long total, free, cached;
		char *match;

		if (!pread("/proc/meminfo", buf, sizeof(buf) - 1)) {
			return NULL;
		}

		if ((match = strstr(buf, "SwapTotal")) == NULL)
			return NULL;
		sscanf(match, "SwapTotal: %ld kB\n", &total);

		if ((match = strstr(buf, "SwapCached")) == NULL)
			return NULL;
		sscanf(match, "SwapCached: %ld kB\n", &cached);

		if ((match = strstr(buf, "SwapFree")) == NULL)
			return NULL;
		sscanf(match, "SwapFree: %ld kB\n", &free);

		return bprintf("%f", (float)(total - free - cached) / 1024 / 1024);
	}
#elif defined(__OpenBSD__)
	#include <stdlib.h>
	#include <sys/param.h> /* dbtob */
	#include <sys/swap.h>
	#include <sys/types.h>
	#include <unistd.h>

	#define	dbtoqb(b) dbtob((int64_t)(b))

	static void
	getstats(int *total, int *used)
	{
		struct swapent *sep, *fsep;
		int rnswap, nswap, i;

		nswap = swapctl(SWAP_NSWAP, 0, 0);
		if (nswap < 1)
			fprintf(stderr, "swaptctl 'SWAP_NSWAP': %s\n", strerror(errno));

		fsep = sep = calloc(nswap, sizeof(*sep));
		if (sep == NULL)
			fprintf(stderr, "calloc 'nswap': %s\n", strerror(errno));

		rnswap = swapctl(SWAP_STATS, (void *)sep, nswap);
		if (rnswap < 0)
			fprintf(stderr, "swapctl 'SWAP_STATA': %s\n", strerror(errno));

		if (nswap != rnswap)
			fprintf(stderr, "SWAP_STATS != SWAP_NSWAP\n");

		*total = 0;
		*used = 0;

		for (i = 0; i < rnswap; i++) {
			*total += dbtoqb(sep->se_nblks);
			*used += dbtoqb(sep->se_inuse);
		}

		free(fsep);
	}

	const char *
	swap_free(void)
	{
		int total, used;

		getstats(&total, &used);

		return bprintf("%f", (float)(total - used) / 1024 / 1024 / 1024);
	}

	const char *
	swap_perc(void)
	{
		int total, used;

		getstats(&total, &used);

		return bprintf("%d", 100 * used / total);
	}

	const char *
	swap_total(void)
	{
		int total, used;

		getstats(&total, &used);

		return bprintf("%f", (float)total / 1024 / 1024 / 1024);
	}

	const char *
	swap_used(void)
	{
		int total, used;

		getstats(&total, &used);

		return bprintf("%f", (float)used / 1024 / 1024 / 1024);
	}
#endif
