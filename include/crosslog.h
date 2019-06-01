#ifndef CROSSLOG_H
#define CROSSLOG_H

#ifndef CROSSLOG_TAG
#define CROSSLOG_TAG "default"
#endif

#if defined(__ZEPHYR__)
    #include <misc/printk.h>
    #include <kernel.h>

    #define CROSSLOG_INTERNAL(prefix, fmt, ...) \
        printk(prefix " " CROSSLOG_TAG ": " fmt "\n", ##__VA_ARGS__)

    #define CROSSLOGV(fmt, ...) CROSSLOG_INTERNAL("V", fmt, ##__VA_ARGS__)
    #define CROSSLOGD(fmt, ...) CROSSLOG_INTERNAL("D", fmt, ##__VA_ARGS__)
    #define CROSSLOGI(fmt, ...) CROSSLOG_INTERNAL("I", fmt, ##__VA_ARGS__)
    #define CROSSLOGW(fmt, ...) CROSSLOG_INTERNAL("W", fmt, ##__VA_ARGS__)
    #define CROSSLOGE(fmt, ...) CROSSLOG_INTERNAL("E", fmt, ##__VA_ARGS__)

    #define CROSSLOG_ABORT() k_panic()

#elif defined(ESP_PLATFORM)
    #include <esp_log.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>

    #define CROSSLOGV(fmt, ...) ESP_LOGV(CROSSLOG_TAG, fmt, ##__VA_ARGS__)
    #define CROSSLOGD(fmt, ...) ESP_LOGD(CROSSLOG_TAG, fmt, ##__VA_ARGS__)
    #define CROSSLOGI(fmt, ...) ESP_LOGI(CROSSLOG_TAG, fmt, ##__VA_ARGS__)
    #define CROSSLOGW(fmt, ...) ESP_LOGW(CROSSLOG_TAG, fmt, ##__VA_ARGS__)
    #define CROSSLOGE(fmt, ...) ESP_LOGE(CROSSLOG_TAG, fmt, ##__VA_ARGS__)

    #define CROSSLOG_ERRNO(fmt, ...) CROSSLOGE(fmt ": %s(%d)", ##__VA_ARGS__, strerror(errno), errno)
    #define CROSSLOG_ABORT() abort()

#elif defined(__unix__)
    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>
    #include <stdlib.h>

    #define CROSSLOG_COLOR_RESET "\x1B[0m"
    #define CROSSLOG_COLOR_PREFIX "\x1B[38;5;"
    #define CROSSLOG_COLOR_BLUE    CROSSLOG_COLOR_PREFIX "75m"
    #define CROSSLOG_COLOR_GREEN   CROSSLOG_COLOR_PREFIX "40m"
    #define CROSSLOG_COLOR_ORANGE  CROSSLOG_COLOR_PREFIX "202m"
    #define CROSSLOG_COLOR_RED     CROSSLOG_COLOR_PREFIX "196m"
    #define CROSSLOG_COLOR_WHITE   CROSSLOG_COLOR_PREFIX "15m"
    #define CROSSLOG_COLOR(x) (isatty(STDERR_FILENO) ? CROSSLOG_COLOR_ ## x : "")

    #define CROSSLOG_INTERNAL(prefix, color, fmt, ...) \
        fprintf(stderr, "%s" prefix " " CROSSLOG_TAG ": " fmt "%s\n", \
            CROSSLOG_COLOR(color), ##__VA_ARGS__, CROSSLOG_COLOR(RESET) \
        )

    #define CROSSLOGV(fmt, ...) CROSSLOG_INTERNAL("V", WHITE, fmt, ##__VA_ARGS__)
    #define CROSSLOGD(fmt, ...) CROSSLOG_INTERNAL("D", BLUE, fmt, ##__VA_ARGS__)
    #define CROSSLOGI(fmt, ...) CROSSLOG_INTERNAL("I", GREEN, fmt, ##__VA_ARGS__)
    #define CROSSLOGW(fmt, ...) CROSSLOG_INTERNAL("W", ORANGE, fmt, ##__VA_ARGS__)
    #define CROSSLOGE(fmt, ...) CROSSLOG_INTERNAL("E", RED, fmt, ##__VA_ARGS__)

    #define CROSSLOG_ERRNO(fmt, ...) CROSSLOGE(fmt ": %s(%d)", ##__VA_ARGS__, strerror(errno), errno)
    #define CROSSLOG_ABORT() abort()
#else
    #error "unsupported platform"
#endif

#define CROSSLOG_ASSERT(x) if(!(x)) { CROSSLOGE("Assertion failed: %s", #x); CROSSLOG_ABORT(); }
#define CROSSLOG_HEXDUMP(buf, sz) _crosslog_hexdump(buf, sz)

void _crosslog_hexdump(const void *buf, size_t sz);

#endif /* CROSSLOG_H */
