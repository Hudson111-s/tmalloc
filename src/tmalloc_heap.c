#include "tmalloc_heap.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static TimedMalloc *heap = NULL;
static size_t heap_capacity = 0;
static size_t heap_size = 0;

static void swap(TimedMalloc *tm1, TimedMalloc *tm2) {
    TimedMalloc temp = *tm1;
    *tm1 = *tm2;
    *tm2 = temp;
}

static int heap_up(int index) {
    int swapped = 0;

    while (index > 0) {
        int parent_index = (index - 1) / 2;
        if (heap[parent_index].lifetime_end <= heap[index].lifetime_end) break;

        swap(&heap[parent_index], &heap[index]);
        swapped = 1;
        index = parent_index;
    }

    return swapped;
}

static int heap_down(int index) {
    int swapped = 0;

    while (1) {
        int left = (index * 2) + 1;
        int right = (index * 2) + 2;
        int smallest = index;

        if (heap_size > (size_t)left && heap[index].lifetime_end > heap[left].lifetime_end) {
            smallest = left;
        } else if (heap_size > (size_t)right && heap[index].lifetime_end > heap[right].lifetime_end) {
            smallest = right;
        }
        if (smallest == index) break;

        swap(&heap[smallest], &heap[index]);
        swapped = 1;
        index = smallest;
    }

    return swapped;
}

TimedMalloc heap_pop() {
    if (heap_size == 0) return (TimedMalloc){0};

    TimedMalloc popped = heap[0];
    heap_size--;

    if (heap_size > 0) {
        heap[0] = heap[heap_size - 1];
        heap_down(0);
    }
    
    return popped;
}

int heap_push(TimedMalloc tm) {
    if (heap_size == heap_capacity) {
        size_t new_capacity = heap_capacity > 0 ? heap_capacity * 2 : HEAP_INIT_CAPACITY;
        TimedMalloc *new_heap = (TimedMalloc *)realloc(heap, sizeof(TimedMalloc) * new_capacity);
        if (new_heap == NULL) return -1;

        heap = new_heap;
        heap_capacity = new_capacity;
    }

    heap[heap_size] = tm;
    heap_up((int)heap_size);
    heap_size++;

    return 0;
}

void heap_free(void *ptr) {
    for (int i = 0; (size_t)i < heap_size; i++) {
        if (heap[i].ptr == ptr) {
            heap[i] = heap[--heap_size];

            if (heap_up(i)) return;
            heap_down(i);
        }
    }
}

TimedMalloc heap_peek() {
    if (heap_size == 0) return (TimedMalloc){0};
    return heap[0];
}

size_t get_heap_size() {
    return heap_size;
}
