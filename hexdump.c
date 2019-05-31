#define CROSSLOG_TAG "hexdump"

#include <crosslog.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

static inline int local_isprint(int c) {
    return ((c >= 0x20) && (c < 0x7f));
}

static ssize_t easy_snprintf(char *s, size_t n, const char *format, ...) {
    int rc;

    va_list args;
    va_start(args, format);
    rc = vsnprintf(s, n, format, args);
    va_end (args);

    if (rc < 0 || (size_t)rc >= n) {
        return -1;
    }

    return (ssize_t)rc;
}

#define BUF_SNPRINTF(buf, off, fmt, ...) easy_snprintf((buf) + (off), ARRAY_SIZE(buf) - (off), fmt, ##__VA_ARGS__)

#define _HEXDUMP_APPENDLINE(fmt, ...) do { \
    nbytes = BUF_SNPRINTF(line, linesz, fmt, ##__VA_ARGS__); \
    if (nbytes < 0) \
        goto err; \
    linesz += nbytes; \
} while(0)

void _crosslog_hexdump(const void *_buf, size_t sz) {
    char line[79];
    size_t i;
    size_t j;
    const uint8_t *buf = _buf;
    ssize_t nbytes;

    (void)(buf);

    for (i = 0; i < sz; i += 16) {
        size_t linesz = 0;
        size_t toread = MIN(16, sz - i);

        // offset
        _HEXDUMP_APPENDLINE("%08zx ", i);

        // hexdump
        for (j = 0; j < 16; j++) {
            if (j < toread) {
                _HEXDUMP_APPENDLINE(" %s%02x", j==8?" ":"", buf[i + j]);
            }
            else {
                _HEXDUMP_APPENDLINE("%s   ", j==8?" ":"");
            }
        }

        _HEXDUMP_APPENDLINE("  |");

        // ascii
        for (j = 0; j < 16; j++) {
            if (j < toread) {
                _HEXDUMP_APPENDLINE("%c", local_isprint(buf[i + j])?buf[i + j]:'.');
            }
            else {
                _HEXDUMP_APPENDLINE(" ");
            }
        }

        _HEXDUMP_APPENDLINE("|");

        CROSSLOGV("%s", line);
    }

    return;

err:
    CROSSLOGV("internal hexdump error");
}
#undef _HEXDUMP_APPENDLINE
