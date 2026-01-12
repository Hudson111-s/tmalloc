#include "tmalloc.h"
#include "tmalloc_time.h"
#include "tmalloc_heap.h"
#include "tmalloc_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

static tm_mutex_t lock;
static tm_once_init_t thread_once = TM_ONCE_INIT;

static void reaper(void) {
    while(1) {
        mutex_lock(&lock);
        while (get_heap_size() > 0 && heap_peek().lifetime_end <= time_ms()) {
            TimedMalloc ex = heap_pop();
            mutex_unlock(&lock);
            free(ex.ptr);
            mutex_lock(&lock);
        }

        uint64_t sleep_tms = SLEEP_INIT;
        uint64_t next_lifetime = UINT64_MAX;
        if (get_heap_size() > 0) {
            next_lifetime = heap_peek().lifetime_end;
            sleep_tms = (next_lifetime > time_ms()) ? next_lifetime - time_ms() : SLEEP_MIN;
        }
        mutex_unlock(&lock);

        if (sleep_tms > 2) sleep_ms(sleep_tms - 1);
        while (1) {
            mutex_lock(&lock);
            int empty = (get_heap_size() == 0);
            next_lifetime = heap_peek().lifetime_end;
            mutex_unlock(&lock);

            if (empty || time_ms() >= next_lifetime) break;
            cpu_pause();
        }
    }
}

static void tm_global_init(void) {
    mutex_init(&lock); 
    reaper_init(reaper);
}

void *tmalloc(size_t size, int64_t lifetime_ms) {
    if (lifetime_ms <= 0) return NULL;
    if (run_thread_once(&thread_once, tm_global_init) != 1) return NULL;

    void *ptr = malloc(size);
    if (ptr == NULL) return NULL;

    TimedMalloc tm = {time_ms() + (uint64_t)lifetime_ms, ptr};
    mutex_lock(&lock);
    if (heap_push(tm) != 0) {
        mutex_unlock(&lock);
        free(ptr);
        return NULL;
    }
    mutex_unlock(&lock);

    return ptr;
}

void tfree(void *ptr) {
    if (ptr == NULL) return;
    if (run_thread_once(&thread_once, tm_global_init) != 1) return;

    mutex_lock(&lock);
    heap_free(ptr);
    mutex_unlock(&lock);

    free(ptr);
}
