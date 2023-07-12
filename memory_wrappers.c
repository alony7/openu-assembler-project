#include <stdlib.h>
#include "memory_wrappers.h"
#include "consts.h"
void *safe_malloc(unsigned int size) {
    /* allocate memory */
    void *ptr = malloc(size);
    /* check if allocation succeeded */
    if (ptr == NULL) {
        /* allocation failed, throw error and exit */
        throw_system_error("Memory allocation Failed. Exiting....\n", FALSE);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *safe_realloc(void *ptr, unsigned int size) {
    /* allocate memory */
    void *new_ptr = realloc(ptr, size);
    /* check if allocation succeeded */
    if (new_ptr == NULL) {
        /* allocation failed, throw error and exit */
        throw_system_error("Memory reallocation Failed. Exiting....\n", FALSE);
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}