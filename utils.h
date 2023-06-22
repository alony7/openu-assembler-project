#pragma once
#include "io_parsers.h"

typedef enum Bool {
    FALSE = 0,
    TRUE = 1
} Bool;

char *string_array_to_string(char **array, int size);

Bool validate_operand_parameters_count(OperandRow *row, int parameters_count);
