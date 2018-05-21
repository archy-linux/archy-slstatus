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

int
esnprintf(char *str, size_t size, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = vsnprintf(str, size, fmt, ap);
	va_end(ap);

	if (ret < 0) {
		warn("snprintf:");
		return -1;
	} else if ((size_t)ret >= size) {
		warn("snprintf: Output truncated");
		return -1;
	}

	return ret;
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
fmt_human(size_t num, int base)
{
	size_t i;
	double scaled;
	const char *siprefix[] = { "", "k", "M", "G", "T", "P", "E", "Z", "Y" };
	const char *iecprefix[] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei",
	                            "Zi", "Yi" };
	char *prefix[9];

	if (base == 1000) {
		memcpy(prefix, siprefix, sizeof(prefix));
	} else if (base == 1024) {
		memcpy(prefix, iecprefix, sizeof(prefix));
	}

	scaled = num;
	for (i = 0; i < LEN(prefix) && scaled >= 1024; i++) {
		scaled /= base;
	}

	return bprintf("%.1f%s", scaled, prefix[i]);
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
