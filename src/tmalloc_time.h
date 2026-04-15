#pragma once

#include <stdint.h>

#define WAIT_THRESHOLD 15

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

static LARGE_INTEGER freq;
static INIT_ONCE once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK init_freq(PINIT_ONCE InitOnce, PVOID Param, PVOID *Context) {
    QueryPerformanceFrequency(&freq);
    return TRUE;
}

/* Gets time in milliseconds since boot. */
static inline uint64_t time_ms() {
    InitOnceExecuteOnce(&once, init_freq, NULL, NULL);

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (uint64_t)((counter.QuadPart * 1000) / freq.QuadPart);
}

#else
#include <time.h>

/* Gets time in milliseconds since boot. */
static inline uint64_t time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)(ts.tv_nsec / 1000000);
}

#endif
