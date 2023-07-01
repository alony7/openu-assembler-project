#include <stdio.h>
#include <malloc.h>
#include "error.h"


void throw_program_error(int line_number, char *error_message, char *file_name, Bool should_free_smg) {
    fprintf(stderr, "Error in file '%s' at line %d: %s\n", file_name, line_number, error_message);
    if (should_free_smg) {
        free(error_message);
    }
}

void throw_system_error(char *error_message, Bool should_free_smg) {
    fprintf(stderr, "%s\n", error_message);
    if (should_free_smg) {
        free(error_message);
    }
}

