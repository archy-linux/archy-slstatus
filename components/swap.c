/* See LICENSE file for copyright and license details. */
#if defined(__linux__)
	#include <errno.h>
	#include <stdio.h>
	#include <string.h>

	#include "../util.h"

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
	/* unimplemented */
#endif
