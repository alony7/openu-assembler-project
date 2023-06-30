#pragma once

typedef struct ErrContext {
    char *file_name;
    int line_number;
} ErrContext;

void export_error(int line_number, char *error_message, char *file_name);
