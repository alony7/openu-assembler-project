#pragma once
#include "io_parsers.h"
#include "instruction_handling.h"

typedef enum Bool {
    FALSE = 0,
    TRUE = 1
} Bool;

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

Bool is_integer(char *str);

Bool is_comment(char *operand);

Bool is_empty_row(char *row);

Opcode get_opcode(char *command);

Register get_register(char *operand);

Bool is_register(char *operand);

Bool is_label(char *operand);

int parse_int(char *str);

void code_number_into_word_bits(Word *word, int number, int offset, int length);

OpcodeMode get_opcode_possible_modes(Opcode opcode);