#include <stdio.h>
#include <malloc.h>
#include "error.h"
#include "utils.h"




void throw_system_error(char *error_message, Bool should_free_smg) {
    system_log(ERROR, error_message, should_free_smg);
}


void add_error_line(ErrorList *list, int line_number) {
    list->size++;
    if(list->lines == NULL)
        /* if this is the first line, allocate memory for the lines array */
        list->lines = (int *) malloc(sizeof(int));
    else{
        /* otherwise, reallocate the memory for the lines array */
        list->lines = (int *) realloc(list->lines, list->size * sizeof(int));
    }
    /* add the line number to the list */
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
    /* iterate over the lines in the list */
    for (i = 0; i < list->size; i++) {
        /* if the line number is in the list, return TRUE */
        if (list->lines[i] == line_number) {
            return TRUE;
        }
    }

    return FALSE;
}
