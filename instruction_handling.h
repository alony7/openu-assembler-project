#pragma once

#include "util_types.h"
#include "io_parsers.h"
#include "error.h"

/**
 * Enum representing all possible instruction codes.
 */
typedef enum InstructionCode {
    /* Move instruction */
    MOV = 0,
    /* Compare instruction */
    CMP = 1,
    /* Add instruction */
    ADD = 2,
    /* Subtract instruction */
    SUB = 3,
    /* Bitwise NOT instruction */
    NOT = 4,
    /* Clear instruction */
    CLR = 5,
    /* Load effective address instruction */
    LEA = 6,
    /* Increment instruction */
    INC = 7,
    /* Decrement instruction */
    DEC = 8,
    /* Jump instruction */
    JMP = 9,
    /* Branch if not equal instruction */
    BNE = 10,
    /* Read input instruction */
    RED = 11,
    /* Print instruction */
    PRN = 12,
    /* Jump to subroutine instruction */
    JSR = 13,
    /* Return from subroutine instruction */
    RTS = 14,
    /* Stop instruction */
    STOP = 15,
    /* Invalid instruction */
    INVALID_INSTRUCTION = -1
} InstructionCode;

/**
 * Structure representing a word consisting of boolean bits.
 */
typedef struct {
    /* The bits of the word */
    Bool bits[WORD_SIZE];
} Word;

/**
 * Enum representing the context of an operand (source or destination).
 */
typedef enum OperandContext {
    SOURCE,
    DESTINATION
} OperandContext;

/**
 * Enum representing the types of instructions.
 */
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

/**
 * Enum representing the addressing types.
 */
typedef enum AddressingType {
    IMMEDIATE = 1,
    DIRECT = 3,
    REGISTER = 5,
    NO_VALUE = 0
} AddressingType;


/**
 * Enum representing the addressing methods.
 */
typedef enum AddressingMethod {
    ABSOLUTE_ADDRESSING = 0,
    EXTERNAL_ADDRESSING = 1,
    RELOCATABLE_ADDRESSING = 2
} AddressingMethod;

/**
 * Structure representing the possible addressing modes for a parameter.
 */
typedef struct ParameterMode {
    /* can the parameter be addressed immediately */
    Bool is_immediate;
    /* can the parameter be addressed directly */
    Bool is_direct;
    /* can the parameter be addressed via a register */
    Bool is_register;
} ParameterMode;

/**
 * Structure representing the different addressing modes for parameters of an instruction.
 */
typedef struct InstructionOptions {
    /* the addressing modes for the source parameter */
    ParameterMode src_op;
    /* the addressing modes for the destination parameter */
    ParameterMode dest_op;
} InstructionOptions;

/**
 * Enum representing the register numbers.
 */
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

/**
 * Retrieves the instruction type based on the given token representing the instruction.
 *
 * @param instruction The instruction string.
 * @return The instruction type.
 */
InstructionType get_instruction_type(char *instruction);

/**
 * Addresses a code instruction and updates the code image.
 *
 * @param line The parsed line representing the instruction.
 * @param code_image The code image to update.
 * @param ic The instruction counter.
 * @return TRUE if the addressing was successful, FALSE otherwise.
 */
Bool address_code_instruction(ParsedLine *line, Word *code_image, int *ic);

/**
 * Addresses a string instruction and updates the data image.
 *
 * @param line The parsed line representing the instruction.
 * @param data_image The data image to update.
 * @param dc The data counter.
 * @return TRUE if the addressing was successful, FALSE otherwise.
 */
Bool address_string_instruction(ParsedLine *line, Word *data_image, int *dc);

/**
 * Addresses a data instruction and updates the data image.
 *
 * @param line The parsed line representing the instruction.
 * @param data_image The data image to update.
 * @param dc The datacounter.
 * @return TRUE if the addressing was successful, FALSE otherwise.
 */
Bool address_data_instruction(ParsedLine *line, Word *data_image, int *dc);

/**
 * Advances the line operands by updating the main operand and shifting the parameters.
 * This execution changes the parsed line.
 *
 * @param line The parsed line to advance.
 */
void advance_line_operands(ParsedLine *line);

/**
 * Retrieves the addressing types from a word based on its encoded bits.
 *
 * @param word The word to retrieve the addressing types from.
 * @param src The addressing type of the source operand.
 * @param dest The addressing type of the destination operand.
 */
void get_word_addressing_types(Word *word, AddressingType *src, AddressingType *dest);

/**
 * Parses a symbol into a word based on the symbol index and addressing method.
 *
 * @param symbol_address The address of the symbol in memory image.
 * @param word The word to parse the symbol into.
 * @param addressing_method The addressing method to use.
 */
void parse_symbol_to_word(int symbol_address, Word *word, AddressingMethod addressing_method);



/**
 * Checks if the line has valid commas based on the number of non-parameter tokens.
 *
 * @param line the string to check.
 * @param error_message The error message to update if commas are invalid.
 * @param non_parameter_tokens_count The number of tokens to skip before expecting a comma. This is used to skip the instruction and label tokens
 * @return TRUE if the commas are valid, FALSE otherwise.
 */
Bool is_valid_commas(char *line, char *error_message, int non_parameter_tokens_count);
