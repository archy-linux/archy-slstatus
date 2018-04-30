/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../util.h"

inline int
load_uvmexp(struct uvmexp *uvmexp)
{
	int uvmexp_mib[] = {CTL_VM, VM_UVMEXP};
	size_t size;

	size = sizeof(*uvmexp);

	return sysctl(uvmexp_mib, 2, uvmexp, &size, NULL, 0) >= 0 ? 1 : 0;
}

const char *
ram_free(void)
{
	struct uvmexp uvmexp;
	float free;
	int free_pages;

	if (load_uvmexp(&uvmexp)) {
		free_pages = uvmexp.npages - uvmexp.active;
		free = (double) (free_pages * uvmexp.pagesize) / 1024 / 1024 / 1024;
		return bprintf("%f", free);
	}

	return NULL;
}

const char *
ram_perc(void)
{
	struct uvmexp uvmexp;
	int percent;

	if (load_uvmexp(&uvmexp)) {
		percent = uvmexp.active * 100 / uvmexp.npages;
		return bprintf("%d", percent);
	}

	return NULL;
}

const char *
ram_total(void)
{
	struct uvmexp uvmexp;
	float total;

	if (load_uvmexp(&uvmexp)) {
		total = (double) (uvmexp.npages * uvmexp.pagesize) / 1024 / 1024 / 1024;
		return bprintf("%f", total);
	}

	return NULL;
}

const char *
ram_used(void)
{
	struct uvmexp uvmexp;
	float used;

	if (load_uvmexp(&uvmexp)) {
		used = (double) (uvmexp.active * uvmexp.pagesize) / 1024 / 1024 / 1024;
		return bprintf("%f", used);
	}

	return NULL;
}
