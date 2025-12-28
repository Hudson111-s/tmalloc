#include "tmalloc.h"
#include "tmalloc_time.h"
#include "tmalloc_heap.h"
#include "tmalloc_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

static tm_mutex_t lock;
static tm_once_init_t thread_once = TM_ONCE_INIT;

static void reaper(void) {
    while(1) {
        uint64_t now = time_ms();
        
        mutex_lock(&lock);
        while (get_heap_size() > 0 && heap_peek().lifetime_end <= now) {
            TimedMalloc ex = heap_pop();
            free(ex.ptr);
        }

        uint64_t sleep_tms = SLEEP_INIT;
        uint64_t next_lifetime = UINT64_MAX;
        if (get_heap_size() > 0) {
            next_lifetime = heap_peek().lifetime_end;
            sleep_tms = (next_lifetime > now) ? next_lifetime - now : SLEEP_MIN;
        }
        mutex_unlock(&lock);

        if (sleep_tms > 2) sleep_ms(sleep_tms - 1);
        while (get_heap_size() > 0 && time_ms() < next_lifetime) {
            cpu_pause();
        }
    }
}

static void tm_global_init(void) {
    mutex_init(&lock); 
    init_reaper(reaper);
}

void *tmalloc(size_t size, uint64_t lifetime_ms) {
    if (run_thread_once(&thread_once, tm_global_init) != 1) return NULL;

    void *ptr = malloc(size);
    if (ptr == NULL) return NULL;

    TimedMalloc tm = {ptr, time_ms() + lifetime_ms};
    mutex_lock(&lock);
    if (heap_push(tm) != 0) {
        free(ptr);
        mutex_unlock(&lock);
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

bool tvalid(void *ptr) {
    if (ptr == NULL) return 0;
    if (run_thread_once(&thread_once, tm_global_init) != 1) return 0;

    mutex_lock(&lock);
    bool found = heap_contains(ptr);
    mutex_unlock(&lock);
    return found;
}
