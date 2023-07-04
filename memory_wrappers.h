#pragma once
#include <malloc.h>
#include "error.h"

void *safe_malloc(unsigned int size);

void *safe_realloc(void *ptr, unsigned int size);