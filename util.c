/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *argv0;

static void
verr(const char *fmt, va_list ap)
{
	if (argv0 && strncmp(fmt, "usage", sizeof("usage") - 1)) {
		fprintf(stderr, "%s: ", argv0);
	}

	vfprintf(stderr, fmt, ap);

	if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}
}

void
warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verr(fmt, ap);
	va_end(ap);
}

void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verr(fmt, ap);
	va_end(ap);

	exit(1);
}

const char *
bprintf(const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	if ((ret = vsnprintf(buf, sizeof(buf), fmt, ap)) < 0) {
		warn("vsnprintf:");
	} else if ((size_t)ret >= sizeof(buf)) {
		warn("vsnprintf: Output truncated");
	}
	va_end(ap);

	return buf;
}

const char *
fmt_scaled(size_t bytes)
{
	unsigned int i;
	float scaled;
	const char *units[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB",
	                        "ZiB", "YiB" };

	scaled = bytes;
	for (i = 0; i < LEN(units) && scaled >= 1024; i++) {
		scaled /= 1024.0;
	}

	return bprintf("%.1f%s", scaled, units[i]);
}

int
pscanf(const char *path, const char *fmt, ...)
{
	FILE *fp;
	va_list ap;
	int n;

	if (!(fp = fopen(path, "r"))) {
		warn("fopen '%s':", path);
		return -1;
	}
	va_start(ap, fmt);
	n = vfscanf(fp, fmt, ap);
	va_end(ap);
	fclose(fp);

	return (n == EOF) ? -1 : n;
}
