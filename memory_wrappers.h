#pragma once
#include <malloc.h>
#include "error.h"


/**
 * Safely allocates memory using malloc.
 *
 * @param size The size of the memory to allocate.
 * @return A pointer to the allocated memory.
 * @throws system_error if memory allocation fails.
 */
void *safe_malloc(unsigned int size);

/**
 * Safely reallocates memory using realloc.
 *
 * @param ptr A pointer to the memory to reallocate.
 * @param size The new size of the memory.
 * @return A pointer to the reallocated memory.
 * @throws system_error if memory reallocation fails.
 */
void *safe_realloc(void *ptr, unsigned int size);
