#pragma once
#include "io_parsers.h"
#include "instruction_handling.h"
#include "util_types.h"


char *string_array_to_string(char **array, int size);

char* duplicate_string(const char* str);

Bool is_integer(char *str);

Bool is_comment(char *operand);

Bool is_empty_row(char *row);

Opcode get_opcode(char *command);

Register get_register(char *operand);

Bool is_register(char *operand);

Bool is_label(char *operand);

int parse_int(char *str);

char *join_strings(int num_strings, ...);

