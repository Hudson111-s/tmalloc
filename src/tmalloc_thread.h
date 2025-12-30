#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

#define TM_ONCE_INIT INIT_ONCE_STATIC_INIT

typedef CRITICAL_SECTION tm_mutex_t;
typedef INIT_ONCE tm_once_init_t;

#else
#include <pthread.h>

#define TM_ONCE_INIT PTHREAD_ONCE_INIT

typedef pthread_mutex_t tm_mutex_t;
typedef pthread_once_t tm_once_init_t;

#endif

void mutex_init(tm_mutex_t *m);
void mutex_lock(tm_mutex_t *m);
void mutex_unlock(tm_mutex_t *m);
void reaper_init(void (*reaper)(void));
int run_thread_once(tm_once_init_t *flag, void (*func)(void));
