#pragma once
#include <stdint.h>

#define SLEEP_INIT 10 // Default sleep in ms
#define SLEEP_MIN 1

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#ifdef _MSC_VER
#include <immintrin.h>
#endif

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

/* Sleeps for ms milliseconds. */
static inline void sleep_ms(uint64_t ms) {
    if (ms == 0) return;

    static HANDLE hTimer = NULL;
    if (hTimer == NULL) {
        hTimer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    }

    if (hTimer) {
        LARGE_INTEGER due_time;
        due_time.QuadPart = -(int64_t)(ms * 10000); 

        SetWaitableTimer(hTimer, &due_time, 0, NULL, NULL, FALSE);
        WaitForSingleObject(hTimer, INFINITE);
    } else {
        Sleep((DWORD)ms);
    }
}

/* Gets time in milliseconds since boot. */
static inline uint64_t time_ms() {
    static LARGE_INTEGER freq;
    static int initialized = 0;
    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000) / freq.QuadPart);
}

static inline void cpu_pause() {
#ifdef _MSC_VER
    _mm_pause(); // Does not yield
#else
    SwitchToThread();
#endif
}

#else
#include <unistd.h>
#include <sched.h>
#include <time.h>

/* Sleeps for ms milliseconds. */
static inline void sleep_ms(uint64_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL) != 0);
}

/* Gets time in milliseconds. */
static inline uint64_t time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)(ts.tv_nsec / 1000000);
}

static inline void cpu_pause() {
    sched_yield();
}

#endif
