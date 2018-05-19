/* See LICENSE file for copyright and license details. */
extern char buf[1024];

#define LEN(x) (sizeof (x) / sizeof *(x))

extern char *argv0;

void warn(const char *, ...);
void die(const char *, ...);

int esnprintf(char *str, size_t size, const char *fmt, ...);
const char *bprintf(const char *fmt, ...);
const char *fmt_human_2(size_t num, char *unit);
const char *fmt_human_10(size_t num, char *unit);
int pscanf(const char *path, const char *fmt, ...);
