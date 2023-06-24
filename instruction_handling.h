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
    UNKNOWN
} InstructionType;

typedef enum AddressingType {
    IMMEDIATE = 1,
    DIRECT = 3,
    REGISTER = 5,
    NO_ADD_VALUE = 0
} AddressingType;

typedef enum AddressingMethod {
    ABSOLUTE = 0,
    EXTERNAL = 1,
    RELOCATABLE = 2
} AddressingMethod ;

typedef enum Register {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7
} Register;

InstructionType get_instruction_type(char *instruction);

void parse_registers_to_word(Word *word, Register src_register, Register dest_register);

void empty_word(Word *word);

void parse_operand_to_word(Word *word, Opcode opcode,  AddressingType src_op, AddressingType dest_op);

Bool is_register(char *operand);

Register get_register(char *operand);

Bool is_label(char *operand);


Bool address_code_instruction(OperandRow *row, Word *code_image, int *ic);

Bool address_string_instruction(OperandRow *row, Word *data_image, int *dc);

Bool address_data_instruction(OperandRow *row, Word *data_image, int *dc);

int parse_int(char *str);