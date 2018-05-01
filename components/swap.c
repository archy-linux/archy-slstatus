/* See LICENSE file for copyright and license details. */
#if defined(__linux__)
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../util.h"

const char *
swap_free(void)
{
	long total, free;
	FILE *fp;
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen '/proc/meminfo': %s\n", strerror(errno));
		return NULL;
	}

	if ((bytes_read = fread(buf, sizeof(char), sizeof(buf) - 1, fp)) == 0) {
		fprintf(stderr, "fread '/proc/meminfo': %s\n", strerror(errno));
		fclose(fp);
		return NULL;
	}
	fclose(fp);

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
	FILE *fp;
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen '/proc/meminfo': %s\n", strerror(errno));
		return NULL;
	}

	if ((bytes_read = fread(buf, sizeof(char), sizeof(buf) - 1, fp)) == 0) {
		fprintf(stderr, "fread '/proc/meminfo': %s\n", strerror(errno));
		fclose(fp);
		return NULL;
	}
	fclose(fp);

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
	FILE *fp;
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen '/proc/meminfo': %s\n", strerror(errno));
		return NULL;
	}
	if ((bytes_read = fread(buf, sizeof(char), sizeof(buf) - 1, fp)) == 0) {
		fprintf(stderr, "fread '/proc/meminfo': %s\n", strerror(errno));
		fclose(fp);
		return NULL;
	}
	fclose(fp);

	if ((match = strstr(buf, "SwapTotal")) == NULL)
		return NULL;
	sscanf(match, "SwapTotal: %ld kB\n", &total);

	return bprintf("%f", (float)total / 1024 / 1024);
}

const char *
swap_used(void)
{
	long total, free, cached;
	FILE *fp;
	size_t bytes_read;
	char *match;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen '/proc/meminfo': %s\n", strerror(errno));
		return NULL;
	}
	if ((bytes_read = fread(buf, sizeof(char), sizeof(buf) - 1, fp)) == 0) {
		fprintf(stderr, "fread '/proc/meminfo': %s\n", strerror(errno));
		fclose(fp);
		return NULL;
	}
	fclose(fp);

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
#endif
