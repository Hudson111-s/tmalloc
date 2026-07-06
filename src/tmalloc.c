#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "tmalloc.h"
#include "tmalloc_time.h"
#include "tmalloc_heap.h"
#include "tmalloc_thread.h"

static tm_mutex_t lock;
static tm_cond_t cv;
static tm_once_init_t thread_once = TM_ONCE_INIT;

static void reaper(void) {
    mutex_lock(&lock);
    while (1) {
        // Wait until there is at least one allocation.
        while (get_heap_size() == 0) {
            cond_wait(&cv, &lock);
        }

        // Reap all expired allocations.
        while (get_heap_size() > 0) {
            uint64_t now = time_ms();
            uint64_t next_lifetime = heap_peek().lifetime_end;

            if (next_lifetime > now) {
                // Wait until next_lifetime expiration or a signal.
                cond_timedwait(&cv, &lock, next_lifetime);
                continue;
            }

            TimedMalloc ex = heap_pop();
            
            mutex_unlock(&lock);
            free(ex.ptr);
            mutex_lock(&lock);
        }
    }
}

static void tm_global_init(void) {
    mutex_init(&lock); 
    cond_init(&cv);
    reaper_init(reaper);
}

void *tmalloc(size_t size, int64_t lifetime_ms) {
    if (lifetime_ms <= 0) return NULL;
    if (run_thread_once(&thread_once, tm_global_init) != 1) return NULL;

    void *ptr = malloc(size);
    if (ptr == NULL) return NULL;

    TimedMalloc tm = {time_ms() + (uint64_t)lifetime_ms, ptr};
    mutex_lock(&lock);
    uint64_t old_lifetime = (get_heap_size() > 0) ? heap_peek().lifetime_end : UINT64_MAX;
    if (heap_push(tm) != 0) {
        mutex_unlock(&lock);
        free(ptr);
        return NULL;
    }
    uint64_t new_lifetime = heap_peek().lifetime_end;
    // Signal only if earliest lifetime changes.
    if (new_lifetime < old_lifetime) cond_signal(&cv);
    mutex_unlock(&lock);

    return ptr;
}

void tfree(void *ptr) {
    if (ptr == NULL) return;
    if (run_thread_once(&thread_once, tm_global_init) != 1) return;

    mutex_lock(&lock);
    uint64_t old_lifetime = (get_heap_size() > 0) ? heap_peek().lifetime_end : UINT64_MAX;
    int removed = heap_free(ptr);
    uint64_t new_lifetime = (get_heap_size() > 0) ? heap_peek().lifetime_end : UINT64_MAX;
    // Signal only if earliest lifetime changes.
    if (old_lifetime != new_lifetime) cond_signal(&cv);
    mutex_unlock(&lock);

    // Only free if the reaper hasn't already taken it.
    if (removed == 0) {
        free(ptr);
    }
}
