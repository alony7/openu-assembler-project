#pragma once

#include <stdio.h>
#include "consts.h"
#include "util_types.h"

typedef struct ParsedLine {
    /* The original line of assembly code */
    char original_line[MAX_LINE_LENGTH];
    /* The line number of the parsed line */
    int line_number;
    /* The name of the source file */
    char file_name[MAX_FILENAME_LENGTH];
    /* The main operand of the line (usually the first token */
    char *main_operand;
    /* Array of parameters extracted from the line (usually all the following tokens after the main operand */
    char **parameters;
    /* The number of parameters in the line (usually its the amount of tokens - 1) */
    int parameters_count;
} ParsedLine;

/**
 * Creates a file stream with the specified file name and mode.
 *
 * @param file_name The name of the file to open.
 * @param mode The mode in which to open the file.
 * @return A pointer to the file stream if successful, NULL otherwise.
 */
FILE *create_file_stream(char *file_name, char *mode);

/**
 * Builds the output filename by concatenating the base name and the suffix.
 *
 * @param base_name The base name of the file.
 * @param suffix The suffix to append to the base name.
 * @param output_buffer Pointer to the buffer to store the output filename.
 */
void build_output_filename(char *base_name, char *suffix, char *output_buffer);

/**
 * Parses a line of assembly code into a ParsedLine structure.
 *
 * @param line The line of assembly code to parse.
 * @param parsed_line Pointer to the ParsedLine structure to store the parsed information.
 */
void parse_line(char *line, ParsedLine *parsed_line);

/**
 * Frees the memory allocated for a ParsedLine structure.
 *
 * @param line Pointer to the ParsedLine structure to free.
 */
void free_parsed_line(ParsedLine *line);
