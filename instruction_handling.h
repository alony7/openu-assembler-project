#pragma once
#include "utils.h"

//TODO: change to bool
typedef struct
{
    int bits[WORD_SIZE];
} Word;

typedef enum OperandLocation {
    SOURCE,
    DESTINATION
} OperandLocation;

typedef enum InstructionType {
    DATA,
    STRING,
    ENTRY,
    EXTERN,
    COMMAND,
    COMMENT,
    LABEL,
    EMPTY_ROW,
    UNKNOWN
} InstructionType;

typedef enum AddressingType {
    IMMEDIATE = 1,
    DIRECT = 3,
    REGISTER = 5,
    NO_VALUE = 0
} AddressingType;

typedef enum AddressingMethod {
    ABSOLUTE = 0,
    EXTERNAL = 1,
    RELOCATABLE = 2
} AddressingMethod ;

typedef struct ParameterMode {
    Bool is_immediate;
    Bool is_direct;
    Bool is_register;
} ParameterMode;

typedef struct OpcodeModes {
    ParameterMode src_op;
    ParameterMode dest_op;
} OpcodeMode;

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





Bool address_code_instruction(OperandRow *row, Word *code_image, int *ic);

Bool address_string_instruction(OperandRow *row, Word *data_image, int *dc);

Bool address_data_instruction(OperandRow *row, Word *data_image, int *dc);

