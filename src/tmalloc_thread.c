#include "tmalloc_thread.h"
#include "tmalloc_time.h"
#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

void mutex_init(tm_mutex_t *m) {
    InitializeCriticalSection(m);
}

void mutex_lock(tm_mutex_t *m) {
    EnterCriticalSection(m);
}

void mutex_unlock(tm_mutex_t *m) {
    LeaveCriticalSection(m);
}

void cond_init(tm_cond_t *c) {
    InitializeConditionVariable(c);
}

void cond_signal(tm_cond_t *c) {
    WakeConditionVariable(c);
}

void cond_wait(tm_cond_t *c, tm_mutex_t *m) {
    SleepConditionVariableCS(c, m, INFINITE);
}

void cond_timedwait(tm_cond_t *c, tm_mutex_t *m, uint64_t lifetime_end) {
    uint64_t now = time_ms();
    // Convert to relative ms.
    uint64_t wait_tms = (lifetime_end > now) ? lifetime_end - now : 0;
    SleepConditionVariableCS(c, m, (DWORD)wait_tms);
}

static BOOL CALLBACK PinitOnceFn(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
    (void)InitOnce; // Cast to silences warnings
    (void)Context;

    void (*func)(void) = (void (*)(void))Parameter;
    func();
    return TRUE;
}

static DWORD WINAPI reaper_thread_thunk(LPVOID param) {
    void (*reaper)(void) = (void (*)(void))param;
    reaper();
    return 0;
}

void reaper_init(void (*reaper)(void)) {
    HANDLE hThread = CreateThread(
        NULL,
        0, 
        reaper_thread_thunk,
        (LPVOID)reaper,
        0,
        NULL
    );
    if (hThread) CloseHandle(hThread); // detach
}

int run_thread_once(tm_once_init_t *flag, void (*func)(void)) {
    return (InitOnceExecuteOnce(flag, PinitOnceFn, func, NULL) != 0);
}

#else
#include <pthread.h>

void mutex_init(tm_mutex_t *m) {
    pthread_mutex_init(m, NULL);
}

void mutex_lock(tm_mutex_t *m) {
    pthread_mutex_lock(m);
}

void mutex_unlock(tm_mutex_t *m) {
    pthread_mutex_unlock(m);
}

void cond_init(tm_cond_t *c) {
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC); // use monotonic clock
    pthread_cond_init(c, &attr);
    pthread_condattr_destroy(&attr);
}

void cond_signal(tm_cond_t *c) {
    pthread_cond_signal(c);
}

void cond_wait(tm_cond_t *c, tm_mutex_t *m) {
    pthread_cond_wait(c, m);
}

void cond_timedwait(tm_cond_t *c, tm_mutex_t *m, uint64_t lifetime_end) {
    uint64_t now = time_ms();
    // Can do this because lifetime_end is CLOCK_MONOTONIC.
    uint64_t wait_tms = (lifetime_end > now) ? lifetime_end : now;

    struct timespec ts;
    ts.tv_sec = wait_tms / 1000;
    ts.tv_nsec = (wait_tms % 1000) * 1000000;
    pthread_cond_timedwait(c, m, &ts);
}

static void *reaper_thread_thunk(void *arg) {
    void (*reaper)(void) = (void(*)(void))arg;
    reaper();
    return NULL;
}

void reaper_init(void (*reaper)(void)) {
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, reaper_thread_thunk, (void*)reaper) != 0) return;
    pthread_detach(thread_id);
}

int run_thread_once(tm_once_init_t *flag, void (*func)(void)) {
    return (pthread_once(flag, func) == 0);
}

#endif
