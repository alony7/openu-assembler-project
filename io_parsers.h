#pragma once

#include <stdio.h>
#include "consts.h"
#include "util_types.h"

typedef struct ParsedLine {
    char original_line[MAX_LINE_LENGTH];
    int line_number;
    char file_name[MAX_FILENAME_LENGTH];
    char *main_operand;
    char **parameters;
    int parameters_count;
} ParsedLine;


FILE *create_file_stream(char *file_name, char *mode);



void build_output_filename(char *base_name, char *suffix, char *output_buffer);

void parse_line(char *line, ParsedLine *parsed_line);

void free_parsed_line(ParsedLine *line);
