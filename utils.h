#pragma once
#include "io_parsers.h"

typedef enum Bool {
    FALSE = 0,
    TRUE = 1
} Bool;

typedef struct ErrInfo {
    int line_number;
    char *error_message;
    char *file_name;
} ErrInfo;

typedef enum Opcode {
    MOV = 0,
    CMP = 1,
    ADD = 2,
    SUB = 3,
    NOT = 4,
    CLR = 5,
    LEA = 6,
    INC = 7,
    DEC = 8,
    JMP = 9,
    BNE = 10,
    RED = 11,
    PRN = 12,
    JSR = 13,
    RTS = 14,
    STOP = 15,
    INVALID_OPCODE = -1
} Opcode;

char *string_array_to_string(char **array, int size);

char* duplicate_string(const char* str);

void export_error(ErrInfo *err_info);

ErrInfo *create_error_info(int line_number, char *error_message, char *file_name);

Bool is_integer(char *str);

Bool is_comment(char *operand);

Bool is_empty_row(char *row);


