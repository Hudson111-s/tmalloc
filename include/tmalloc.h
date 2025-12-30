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

#ifdef __cplusplus
}
#endif