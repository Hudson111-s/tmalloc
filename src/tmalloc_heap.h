#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define HEAP_INIT_CAPACITY 16

typedef struct TimedMalloc {
    void *ptr;
    uint64_t lifetime_end;    
} TimedMalloc;

size_t get_heap_size();
TimedMalloc heap_peek();
void heap_free(void *ptr);
bool heap_contains(void *ptr);
int heap_push(TimedMalloc tm);
TimedMalloc heap_pop();
