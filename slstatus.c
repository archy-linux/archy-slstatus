/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "arg.h"
#include "slstatus.h"
#include "util.h"

struct arg {
	const char *(*func)();
	const char *fmt;
	const char *args;
};

char *argv0;
char buf[1024];
static int done;
static Display *dpy;

#include "config.h"

static void
terminate(const int signo)
{
	(void)signo;

	done = 1;
}

static void
difftimespec(struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec - (a->tv_nsec < b->tv_nsec);
	res->tv_nsec = a->tv_nsec - b->tv_nsec +
	               (a->tv_nsec < b->tv_nsec) * 1000000000;
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-s]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	struct sigaction act;
	struct timespec start, current, diff, intspec, wait;
	size_t i, len;
	int sflag, ret;
	char status[MAXLEN];
	const char *res;

	sflag = 0;
	ARGBEGIN {
		case 's':
			sflag = 1;
			break;
		default:
			usage();
	} ARGEND

	if (argc) {
		usage();
	}

	memset(&act, 0, sizeof(act));
	act.sa_handler = terminate;
	sigaction(SIGINT,  &act, NULL);
	sigaction(SIGTERM, &act, NULL);

	if (sflag) {
		setbuf(stdout, NULL);
	}

	if (!sflag && !(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "XOpenDisplay: Failed to open display\n");
		return 1;
	}

	while (!done) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			fprintf(stderr, "clock_gettime: %s\n", strerror(errno));
			return 1;
		}

		status[0] = '\0';
		for (i = len = 0; i < LEN(args); i++) {
			if (!(res = args[i].func(args[i].args))) {
				res = unknown_str;
			}
			if ((ret = snprintf(status + len, sizeof(status) - len,
			                    args[i].fmt, res)) < 0) {
				fprintf(stderr, "snprintf: %s\n",
				        strerror(errno));
				break;
			} else if ((size_t)ret >= sizeof(status) - len) {
				fprintf(stderr, "snprintf: Output truncated\n");
				break;
			}
			len += ret;
		}

		if (sflag) {
			printf("%s\n", status);
		} else {
			if (XStoreName(dpy, DefaultRootWindow(dpy), status) < 0) {
				fprintf(stderr,
				        "XStoreName: Allocation failed\n");
				return 1;
			}
			XFlush(dpy);
		}

		if (!done) {
			if (clock_gettime(CLOCK_MONOTONIC, &current) < 0) {
				fprintf(stderr, "clock_gettime: %s\n",
				        strerror(errno));
				return 1;
			}
			difftimespec(&diff, &current, &start);

			intspec.tv_sec = interval / 1000;
			intspec.tv_nsec = (interval % 1000) * 1000000;
			difftimespec(&wait, &intspec, &diff);

			if (wait.tv_sec >= 0) {
				if (nanosleep(&wait, NULL) < 0 &&
				    errno != EINTR) {
					fprintf(stderr, "nanosleep: %s\n",
					        strerror(errno));
					return 1;
				}
			}
		}
	}

	if (!sflag) {
		XStoreName(dpy, DefaultRootWindow(dpy), NULL);
		if (XCloseDisplay(dpy) < 0) {
			fprintf(stderr,
			        "XCloseDisplay: Failed to close display\n");
			return 1;
		}
	}

	return 0;
}
