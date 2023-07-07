#pragma once

#include "util_types.h"

typedef struct ErrorList {
    int size;
    int *lines;
} ErrorList;

void throw_program_error(int line_number, char *error_message, char *file_name, Bool should_free_smg);

void throw_system_error(char *error_message , Bool should_free_smg);

void add_error_line(ErrorList *list, int line_number);

void free_error_list(ErrorList *list);

ErrorList *create_error_list();

Bool is_line_in_error_list(ErrorList *list, int line_number);
