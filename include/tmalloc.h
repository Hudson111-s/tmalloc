#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates memory that is automatically freed after a given lifetime.
 *
 * @param size Number of bytes to allocate
 * @param lifetime_ms Lifetime in milliseconds
 *
 * @return Pointer to allocated memory, or NULL on failure
 */
void *tmalloc(size_t size, uint64_t lifetime_ms);

/**
 * Frees memory previously allocated by tmalloc before it expires.
 *
 * @param ptr Pointer returned by tmalloc
 */
void tfree(void *ptr);

/**
 * Checks whether a pointer returned by tmalloc is still valid.
 *
 * @param ptr Pointer returned by tmalloc
 * @return true if the allocation has not expired or been freed
 */
bool tvalid(void *ptr);

#ifdef __cplusplus
}
#endif