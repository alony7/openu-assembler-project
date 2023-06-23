#pragma once
#include "io_parsers.h"

typedef enum Bool {
    FALSE = 0,
    TRUE = 1
} Bool;

typedef struct ErrInfo {
    int line_number;
    char *error_message;
    char *file_name;
} ErrInfo;

char *string_array_to_string(char **array, int size);

char* duplicate_string(const char* str);

void export_error(ErrInfo *err_info);

ErrInfo *create_error_info(int line_number, char *error_message, char *file_name);