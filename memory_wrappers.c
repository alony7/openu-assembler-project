#include <stdlib.h>
#include "memory_wrappers.h"
#include "consts.h"
void *safe_malloc(unsigned int size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        throw_system_error("FATAL ERROR: Memory allocation Failed. Exiting....\n", FALSE);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *safe_realloc(void *ptr, unsigned int size) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        throw_system_error("FATAL ERROR: Memory reallocation Failed. Exiting....\n", FALSE);
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}