#pragma once

#include <stdio.h>
#include "consts.h"
#include "util_types.h"

typedef struct ParsedLine {
    char original_line[MAX_LINE_LENGTH];
    int line_number;
    char file_name[MAX_FILENAME_LENGTH];
    char *operand;
    char **parameters;
    int parameters_count;
} ParsedLine;

typedef struct FileOperands {
    ParsedLine *lines;
    int size;
    char *file_name;
} FileOperands;

FILE *create_file_stream(char *file_name, char *mode);

void parse_line(char *line, ParsedLine *parsed_line);

FileOperands *parse_lines_from_file(FILE *file, char *file_name);

void free_parsed_line(ParsedLine *line);

void free_file_operands(FileOperands *file_operands);

void build_output_filename(char *base_name, char *suffix, char *output_buffer);

FILE *open_file(char *file_name, char *mode);