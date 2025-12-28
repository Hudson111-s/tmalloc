#include "tmalloc_thread.h"
#include "tmalloc.h"

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

typedef struct { void (*func)(void); } init_once_t;
static BOOL CALLBACK PinitOnceFn(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
    init_once_t *io = (init_once_t *)Parameter;
    io->func();
    return TRUE;
}

static DWORD WINAPI reaper_thread_thunk(LPVOID param) {
    void (*reaper)(void) = (void (*)(void))param;
    reaper();
    return 0;
}

void init_reaper(void (*reaper)(void)) {
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
    init_once_t io = {func};
    return (InitOnceExecuteOnce(flag, PinitOnceFn, &io, NULL) != 0);
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

void init_reaper(void (*reaper)(void)) {
    pthread_t thread_id;
    void *(*reaper_cast)(void *) = (void *(*)(void *))reaper;

    if (pthread_create(&thread_id, NULL, reaper_cast, NULL) != 0) return;
    pthread_detach(thread_id);
}

int run_thread_once(tm_once_init_t *flag, void (*func)(void)) {
    return (pthread_once(flag, func) == 0);
}

#endif
