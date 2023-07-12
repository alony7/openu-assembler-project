#pragma once

#include "util_types.h"

/**
 * Structure representing a list of error lines.
 */
typedef struct ErrorList {
    /* Number of error lines in the list */
    int size;
    /* Array of error line numbers */
    int *lines;
} ErrorList;



/**
 * Throws a system error with the specified error message.
 *
 * @param error_message The error message to display.
 * @param should_free_smg Indicates whether the error message should be freed after use.
 */
void throw_system_error(char *error_message, Bool should_free_smg);

/**
 * Adds a line number to the error list.
 *
 * @param list Pointer to the error list.
 * @param line_number The line number to add to the error list.
 */
void add_error_line(ErrorList *list, int line_number);

/**
 * Frees the memory associated with the error list.
 *
 * @param list Pointer to the error list.
 */
void free_error_list(ErrorList *list);

/**
 * Creates and initializes an error list.
 *
 * @return Pointer to the newly created error list.
 */
ErrorList *create_error_list();

/**
 * Checks if a given line number is in the error list.
 *
 * @param list Pointer to the error list.
 * @param line_number The line number to check.
 * @return TRUE if the line number is in the error list, FALSE otherwise.
 */
Bool is_line_in_error_list(ErrorList *list, int line_number);
