#include <stdio.h>

typedef struct OperandRow {
    char *operand;
    char **parameters;
    int parameters_count;
} OperandRow;

void remove_end_of_line(char *string);

void *parse_operand_row(char *line, OperandRow *parsed_row);

OperandRow *parse_file_to_operand_rows(FILE *file,max_rows);