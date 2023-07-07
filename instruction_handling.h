#pragma once

#include "util_types.h"
#include "io_parsers.h"
#include "error.h"

typedef enum InstructionCode {
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
    INVALID_INSTRUCTION = -1
} InstructionCode;

typedef struct {
    Bool bits[WORD_SIZE];
} Word;

typedef enum OperandContext {
    SOURCE,
    DESTINATION
} OperandContext;

typedef enum InstructionType {
    DATA,
    STRING,
    ENTRY,
    EXTERN,
    COMMAND,
    COMMENT,
    LABEL,
    EMPTY_LINE,
    UNKNOWN
} InstructionType;

typedef enum AddressingType {
    IMMEDIATE = 1,
    DIRECT = 3,
    REGISTER = 5,
    NO_VALUE = 0
} AddressingType;

typedef struct {
    char *name;
    char **value;
    int line_number;
    int value_size;
} MacroItem;

typedef struct {
    MacroItem **items;
    int capacity;
    int size;
} MacroTable;

typedef enum AddressingMethod {
    ABSOLUTE_ADDRESSING = 0,
    EXTERNAL_ADDRESSING = 1,
    RELOCATABLE_ADDRESSING = 2
} AddressingMethod;

typedef struct ParameterMode {
    Bool is_immediate;
    Bool is_direct;
    Bool is_register;
} ParameterMode;

typedef struct InstructionOptions {
    ParameterMode src_op;
    ParameterMode dest_op;
} InstructionOptions;

typedef enum Register {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    INVALID_REGISTER = -1
} Register;


InstructionType get_instruction_type(char *instruction);

Bool address_code_instruction(ParsedLine *line, Word *code_image, int *ic);

Bool address_string_instruction(ParsedLine *line, Word *data_image, int *dc);

Bool address_data_instruction(ParsedLine *line, Word *data_image, int *dc);

void advance_line_operands(ParsedLine *line);

void get_word_addressing_types(Word *word, AddressingType *src, AddressingType *dest);

void parse_symbol_to_word(int symbol_index, Word *word, AddressingMethod addressing_method);

Bool is_valid_commas(char *line, char *error_message, int non_parameter_tokens_count);