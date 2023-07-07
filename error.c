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


void add_error_line(ErrorList *list, int line_number) {
    list->size++;
    if(list->lines == NULL)
        list->lines = (int *) malloc(sizeof(int));
    else{
        list->lines = (int *) realloc(list->lines, list->size * sizeof(int));
    }
    list->lines[list->size-1] = line_number;
}

void free_error_list(ErrorList *list) {
    free(list->lines);
    free(list);
}

ErrorList *create_error_list() {
    ErrorList *list = (ErrorList *) malloc(sizeof(ErrorList));
    list->size = 0;
    list->lines = NULL;
    return list;
}

Bool is_line_in_error_list(ErrorList *list, int line_number) {
    int i;
    for (i = 0; i < list->size; i++) {
        if (list->lines[i] == line_number) {
            return TRUE;
        }
    }
    return FALSE;
}
