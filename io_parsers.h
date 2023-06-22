#pragma once

#include <stdio.h>
#include "consts.h"

typedef struct OperandRow {
    char original_line[MAX_LINE_LENGTH];
    char *operand;
    char **parameters;
    int parameters_count;
} OperandRow;

typedef struct FileOperands {
    OperandRow *rows;
    int size;
} FileOperands;

FILE *create_file_stream(char *file_name, char *mode);

void parse_operand_row(char *line, OperandRow *parsed_row);

FileOperands *parse_file_to_operand_rows(FILE *file);

void free_operand_row(OperandRow *row);

void free_file_operands(FileOperands *file_operands);

