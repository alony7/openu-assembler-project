#pragma once
#include <stdio.h>
#include "consts.h"

typedef struct OperandRow {
    char original_line[MAX_LINE_LENGTH];
    char *operand;
    char **parameters;
    int parameters_count;
} OperandRow;

typedef struct FileOperands{
    OperandRow *rows;
    int size;
} FileOperands;

void remove_end_of_line(char *string);

void parse_operand_row(char *line, OperandRow *parsed_row);

FileOperands *parse_file_to_operand_rows(FILE *file);

void remove_row_from_file_operands(FileOperands *file_operands, int row_index);