
#include "error.h"
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "instruction_handling.h"
#include "utils.h"

/**
 * Checks if an instruction has a destination operand based on its options.
 *
 * @param i_options The instruction options to check.
 * @return TRUE if the instruction options have a destination operand, FALSE otherwise.
 */
static Bool instruction_has_destination(InstructionOptions i_options);

/**
 * Checks if an instruction has a source operand based on its options.
 *
 * @param i_options The instruction options to check.
 * @return TRUE if the instruction options have a source operand, FALSE otherwise.
 */
static Bool instruction_has_source(InstructionOptions i_options);

/**
 * Checks if the addressing types specified in the instruction options are legal.
 * The addressing types should match the conditions specified in the given instruction options.
 *
 * @param i_options The instruction options containing the addressing types.
 * @param src_op The addressing type of the source operand.
 * @param dest_op The addressing type of the destination operand.
 * @return TRUE if the addressing types are legal, FALSE otherwise.
 */
static Bool is_addressing_types_legal(InstructionOptions i_options, AddressingType src_op, AddressingType dest_op);

/**
 * Builds the instruction options based on the provided flags for source and destination operands.
 *
 * @param src_is_immediate Flag indicating if the source operand can be immediate.
 * @param src_is_direct Flag indicating if the source operand can be direct.
 * @param src_is_register Flag indicating if the source operand can be a register.
 * @param dest_is_immediate Flag indicating if the destination operand can be immediate.
 * @param dest_is_direct Flag indicating if the destination operand can be direct.
 * @param dest_is_register Flag indicating if the destination operand can be a register.
 * @param i_options Pointer to the InstructionOptions structure to update.
 */
static void build_instruction_options(int src_is_immediate, int src_is_direct, int src_is_register, int dest_is_immediate, int dest_is_direct, int dest_is_register, InstructionOptions *i_options);

/**
 * Handles the processing of an instruction. the instruction is usually the first token in the line thus making it the main operand.
 *
 * @param line The parsed line representing the instruction.
 * @param code_image The code image to update.
 * @param ic The instruction counter.
 * @param i_code Pointer to the instruction code.
 * @param i_options Pointer to the instruction options.
 * @param raw_src_operand Pointer to the raw source operand string.
 * @param raw_dest_operand Pointer to the raw destination operand string.
 * @param src_op Pointer to the addressing type of the source operand.
 * @param dest_op Pointer to the addressing type of the destination operand.
 * @return TRUE if the instruction handling was successful, FALSE otherwise.
 */
static Bool handle_instruction(const ParsedLine *line, Word *code_image, int *ic, const InstructionCode *i_code, InstructionOptions *i_options, char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op, AddressingType *dest_op);

/**
 * Handles the processing of parameter operands for an instruction.
 *
 * @param line The parsed line representing the instruction.
 * @param code_image The code image to update.
 * @param ic The instruction counter.
 * @param raw_src_operand The raw source operand string.
 * @param raw_dest_operand The raw destination operand string.
 * @param src_op The addressing type of the source operand.
 * @param dest_op The addressing type of the destination operand.
 * @return TRUE if the parameter operand handling was successful, FALSE otherwise.
 */
static Bool handle_parameter_operands(ParsedLine *line, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand, AddressingType src_op, AddressingType dest_op);

/**
 * Parses registers into a word based on the source and destination register numbers.
 *
 * @param word The word to parse the registers into.
 * @param src_register The source register.
 * @param dest_register The destination register.
 */
static void parse_registers_to_word(Word *word, Register src_register, Register dest_register);

/**
 * Parses an assembly instruction into a word based on the instruction code and addressing types of its parameters.
 *
 * @param word The word to parse the operand into.
 * @param i_code The instruction code.
 * @param src_op The addressing type of the source operand.
 * @param dest_op The addressing type of the destination operand.
 */
static void parse_instruction_to_word(Word *word, InstructionCode i_code, AddressingType src_op, AddressingType dest_op);

/**
 * Retrieves the addressing type based on the given operand, represented as a raw token.
 * This method is used for opereands, not for instructions.
 *
 * @param operand The operand string.
 * @return The addressing type.
 */
static AddressingType get_operand_addressing_type(char *operand);

/**
 * Parses an integer into a word based on the given integer and flag indicating whether to add ARE bits or not.
 *
 * @param word pointer to the word to parse the integer into.
 * @param num The integer number.
 * @param add_ARE Flag indicating whether to add ARE bits or not.
 */
static void parse_int_to_word(Word *word, int num, Bool add_ARE);

/**
 * Codes a word from the operand based on the addressing type and operand context.
 *
 * @param line The parsed line representing the instruction.
 * @param code_image The code image to update.
 * @param ic The instruction counter.
 * @param raw_operand The raw operand string.
 * @param addressing_type The addressing type of the operand.
 * @param context The operand context (source or destination).
 * @return TRUE if the coding was successful, FALSE otherwise.
 */
static Bool code_word_from_operand(ParsedLine *line, Word *code_image, const int *ic, char *raw_operand, AddressingType addressing_type, OperandContext context);

/**
 * Sets the bits of a word to zero.
 *
 * @param word The word to set to zero.
 */
static void set_word_to_zero(Word *word);

/**
 * Codes a number into the bits of a word.
 *
 * @param word pointer to the word to code the number into.
 * @param number The number to code.
 * @param offset The offset index where the coding starts.
 * @param length The number of bits to code.
 */
static void code_number_into_word_bits(Word *word, int number, int offset, int length);

/**
 * Retrieves the relevant instruction options based on the instruction code.
 *
 * @param i_code The instruction code.
 * @param i_options Pointer to the InstructionOptions structure to update.
 */
static void get_instruction_options(InstructionCode i_code, InstructionOptions *i_options);

/**
 * Checks if a string operand is legal.

 * @param line The parsed line representing the instruction.
 * @return TRUE if the string operand is legal, FALSE otherwise.
 */
static Bool is_legal_string_operand(ParsedLine *line);


void set_word_to_zero(Word *word) {
    int i;

    for (i = 0; i < WORD_SIZE; i++) {
        /* Set all bits to zero */
        word->bits[i] = FALSE;
    }
}

void parse_symbol_to_word(int symbol_address, Word *word, AddressingMethod addressing_method) {
    /* add ARE bits */
    code_number_into_word_bits(word, addressing_method, ARE_START_INDEX, ARE_END_INDEX);
    /* add symbol index */
    code_number_into_word_bits(word, symbol_address, 2, 10);
}

void parse_registers_to_word(Word *word, Register src_register, Register dest_register) {
    set_word_to_zero(word);
    /* add ARE bits */
    code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, ARE_START_INDEX, ARE_END_INDEX);
    /* add registers */
    code_number_into_word_bits(word, dest_register, 2, 5);
    code_number_into_word_bits(word, src_register, 7, 5);
}

void parse_instruction_to_word(Word *word, InstructionCode i_code, AddressingType src_op, AddressingType dest_op) {
    set_word_to_zero(word);
    /* add ARE bits */
    code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, ARE_START_INDEX, ARE_END_INDEX);
    /* add destination operand addressing type */
    code_number_into_word_bits(word, dest_op, 2, 3);
    /* add instruction code */
    code_number_into_word_bits(word, i_code, 5, 4);
    /* add source operand addressing type */
    code_number_into_word_bits(word, src_op, 9, 3);
}

AddressingType get_operand_addressing_type(char *operand) {
    if (is_integer(operand)) {
        return IMMEDIATE;
    } else if (is_register(operand)) {
        return REGISTER;
    } else {
        /* operand is a symbol */
        return DIRECT;
    }
}

void parse_int_to_word(Word *word, int num, Bool add_ARE) {
    if (add_ARE) {
        /* the number is part of a code instruction */
        code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, 0, 2);
        code_number_into_word_bits(word, num, 2, 10);
    } else {
        /* the number is part of a data instruction */
        code_number_into_word_bits(word, num, 0, 12);
    }
}
/* TODO: allow data and string without a label */
Bool address_data_instruction(ParsedLine *line, Word *data_image, int *dc) {
    char *parsing_error = NULL;
    int i, parsed_number;
    /* Iterate over the parameters and parse them into the data image */
    if(line->parameters_count == 0) {
        program_log(ERROR,line->line_number, ".data command requires at least 1 parameter", line->file_name, FALSE);
        return FALSE;
    }
    for (i = 0; i < line->parameters_count; i++) {
        parsed_number = parse_int(line->parameters[i], MAX_12_BIT_NUMBER, MIN_12_BIT_NUMBER, &parsing_error);
        /* check if there was a parsing error */
        if (parsing_error != NULL) {
            program_log(ERROR,line->line_number, parsing_error, line->file_name, TRUE);
            /* remove the number of words that were already coded */
            dc -= i;
            return FALSE;
        }
        /* code the number into the data image */
        parse_int_to_word(&(data_image[*dc]), parsed_number, FALSE);
        (*dc)++;
    }
    return TRUE;
}

Bool is_legal_string_operand(ParsedLine *line) {
    int i;
    Bool is_comma_after_string = FALSE;
    char *original_string = line->original_line;
    /* first string is the first part of a space separated string */
    char *first_string = line->parameters[0];
    /* last string is the last part of a space separated string */
    char *last_string = line->parameters[line->parameters_count - 1];
    while(*original_string != STRING_BOUNDARY){
        if(*original_string == COMMA){
            program_log(ERROR,line->line_number, "comma before string is illegal", line->file_name, FALSE);
            return FALSE;
        }
        original_string++;
    }

    /* check if the first and last strings are enclosed in double quotes to support spaces in the string */
    if (first_string[0] != STRING_BOUNDARY || LAST_CHAR(last_string) != STRING_BOUNDARY) {
        program_log(ERROR,line->line_number, "first string must be enclosed in double quotes", line->file_name, FALSE);
        return FALSE;
    }else{
        if(strlen(first_string) == 1 && first_string == last_string){
            program_log(ERROR,line->line_number, "first string must be enclosed in double quotes", line->file_name, FALSE);
            return FALSE;
        }
    }
    for (i = 0; i < strlen(line->original_line); i++) {
        if ((line->original_line[i] & ~ASCII_MAX) != 0) {
            /* the string contains non-ascii characters */
            program_log(ERROR,line->line_number, "first string contains non-ascii characters", line->file_name, FALSE);
            return FALSE;
        }
    }
    while(*original_string != NULL_CHAR){
        if(*original_string == COMMA){
            is_comma_after_string = TRUE;
        }
        else if(*original_string == STRING_BOUNDARY){
            is_comma_after_string = FALSE;
        }
        original_string++;
    }
    if(is_comma_after_string){
        program_log(ERROR,line->line_number, "comma after string is illegal", line->file_name, FALSE);
        return FALSE;
    }
    return TRUE;
}

/* TODO: validate there is a check for space after the double quotes */

Bool address_string_instruction(ParsedLine *line, Word *data_image, int *dc) {
    int i;
    char *string_token = line->original_line;
    if(line->parameters_count == 0){
        program_log(ERROR,line->line_number, "no string to encode after the .string command", line->file_name, FALSE);
        return FALSE;
    }
    if (!is_legal_string_operand(line)) return FALSE;
    while(*string_token != STRING_BOUNDARY){
        string_token++;
    }
    trim_string_quotes(string_token);
    for (i = 0; i < strlen(string_token); i++) {
        parse_int_to_word(&(data_image[*dc]), (int) (string_token[i]), FALSE);
        (*dc)++;
    }

    parse_int_to_word(&(data_image[*dc]), (int) (NULL_CHAR), FALSE);
    (*dc)++;
    return TRUE;
}

Bool code_word_from_operand(ParsedLine *line, Word *code_image, const int *ic, char *raw_operand, AddressingType const addressing_type, OperandContext const context) {
    Register reg;
    int parsed_number;
    char *parsing_error = NULL;

    /* if the operand is a number */
    if (addressing_type == IMMEDIATE) {

        parsed_number = parse_int(raw_operand, MAX_10_BIT_NUMBER, MIN_10_BIT_NUMBER, &parsing_error);
        /* check if there was a parsing error */
        if (parsing_error != NULL) {
            program_log(ERROR,line->line_number, parsing_error, line->file_name, TRUE);
            return FALSE;
        }
        /* code the number into the code image */
        parse_int_to_word(code_image + *ic, parsed_number, TRUE);
    }
    /* the operand is a symbol */
    else if (addressing_type == DIRECT) {
        /* set to zero for now */
        set_word_to_zero(&code_image[*ic]);
    }
    /* the operand is a register */
    else {
        /* get the register number */
        reg = get_register(raw_operand);

        /* check if the register is valid */
        if (reg == INVALID_REGISTER) {
            program_log(ERROR,line->line_number, join_strings(2, "invalid register: ", raw_operand), line->file_name, TRUE);
            return FALSE;
        }

        /* determine if the register is the source or destination operand in order to code it correctly */
        if (context == DESTINATION) {
            parse_registers_to_word(&(code_image[*ic]), 0, reg);
        } else {
            parse_registers_to_word(&(code_image[*ic]), reg, 0);
        }
    }
    return TRUE;
}

Bool instruction_has_source(InstructionOptions i_options) {
    /* if the options allow at least one kind of source operand, return true */
    return i_options.src_op.is_direct || i_options.src_op.is_immediate || i_options.src_op.is_register;
}

Bool instruction_has_destination(InstructionOptions i_options) {
    /* if the options allow at least one kind of destination operand, return true */
    return i_options.dest_op.is_direct || i_options.dest_op.is_immediate || i_options.dest_op.is_register;
}

Bool address_code_instruction(ParsedLine *line, Word *code_image, int *ic) {
    char *raw_src_operand = NULL, *raw_dest_operand = NULL;
    AddressingType src_op = {0}, dest_op = {0};
    ParameterMode src_mode = {0}, dest_mode = {0};
    InstructionOptions i_options = {src_mode, dest_mode};
    /* get the instruction number */
    InstructionCode op_num = get_instruction_code(line->main_operand);
    /* check if the instruction is valid */
    if (op_num == INVALID_INSTRUCTION) {
        program_log(ERROR,line->line_number, join_strings(2, "invalid instruction: ", line->main_operand), line->file_name, TRUE);
        return FALSE;
    }
    /* get the instruction options */
    get_instruction_options(op_num, &i_options);
    /* check if the number of operands is valid by checking if the instruction requires a source and/or destination operand */
    if (line->parameters_count != instruction_has_source(i_options) + instruction_has_destination(i_options)) {
        program_log(ERROR,line->line_number, join_strings(2, "invalid number of operands for instruction: ", line->main_operand), line->file_name, TRUE);
        return FALSE;
    }
    /* process the instruction and check if it was successful */
    if (!handle_instruction(line, code_image, ic, &op_num, &i_options, &raw_src_operand, &raw_dest_operand, &src_op, &dest_op)) {
        return FALSE;
    }
    /* increment the instruction counter */
    (*ic)++;

    if (line->parameters_count == 0) return TRUE;
    /* process the parameters and check if it was successful */
    if (!handle_parameter_operands(line, code_image, ic, raw_src_operand, raw_dest_operand, src_op, dest_op)) {
        /* if it wasn't successful, decrement the instruction counter from the previous increment for the instruction */
        (*ic)--;
        return FALSE;
    }

    return TRUE;
}

Bool handle_parameter_operands(ParsedLine *line, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand, AddressingType const src_op, AddressingType const dest_op) {
    Register src_register, dest_register;
    Bool is_success = TRUE;

    if (line->parameters_count == 1) {
        /* if the instruction has only one operand, isolate if its the source or destination operand */
        if ((src_op) == NO_VALUE) {
            /* the operand is a destination operand */
            ASSIGN_LEFT_LOGICAL_AND(is_success, code_word_from_operand(line, code_image, ic, raw_dest_operand, dest_op, DESTINATION));
        } else {
            /* the operand is a source operand */
            ASSIGN_LEFT_LOGICAL_AND(is_success, code_word_from_operand(line, code_image, ic, raw_src_operand, src_op, SOURCE));
        }

        if (!is_success) return FALSE;

        (*ic)++;
    } else {
        /* the instruction has two operands, both source and destination */
        /* code the source operand */
        if (ASSIGN_LEFT_LOGICAL_AND(is_success, code_word_from_operand(line, code_image, ic, raw_src_operand, src_op, SOURCE))) {
            (*ic)++;
            /* if not both operands are registers, add another word for the destination operand */
            if (!(src_op == REGISTER && dest_op == REGISTER)) {
                ASSIGN_LEFT_LOGICAL_AND(is_success, code_word_from_operand(line, code_image, ic, raw_dest_operand, dest_op, DESTINATION));
                if (!is_success) {
                    /* if the first operand was successful but the second wasn't, decrement the instruction counter from the previous increment for the first operand */
                    (*ic)--;
                    return FALSE;
                };
                (*ic)++;
            } else {
                /* if both operands are registers, code them in the same word */
                src_register = get_register(raw_src_operand);
                dest_register = get_register(raw_dest_operand);
                /* check if the registers are valid */
                if (src_register == INVALID_REGISTER || dest_register == INVALID_REGISTER) {
                    if (src_register == INVALID_REGISTER) {
                        program_log(ERROR,line->line_number, join_strings(2, "invalid register: ", raw_src_operand), line->file_name, TRUE);
                    }
                    if (dest_register == INVALID_REGISTER) {
                        program_log(ERROR,line->line_number, join_strings(2, "invalid register: ", raw_dest_operand), line->file_name, TRUE);
                    }
                    (*ic)--;
                    return FALSE;
                }
                /* code the registers in the same word */
                parse_registers_to_word(&(code_image[*ic - 1]), src_register, dest_register);
            }
        }
    }
    return is_success;
}

Bool is_addressing_types_legal(InstructionOptions const i_options, AddressingType const src_op, AddressingType const dest_op) {
    /* check if the addressing types are legal for the instruction */
    if (!i_options.src_op.is_direct && src_op == DIRECT) return FALSE;
    if (!i_options.src_op.is_immediate && src_op == IMMEDIATE) return FALSE;
    if (!i_options.src_op.is_register && src_op == REGISTER) return FALSE;
    if (!i_options.dest_op.is_direct && dest_op == DIRECT) return FALSE;
    if (!i_options.dest_op.is_immediate && dest_op == IMMEDIATE) return FALSE;
    if (!i_options.dest_op.is_register && dest_op == REGISTER) return FALSE;
    return TRUE;
}

Bool handle_instruction(const ParsedLine *line, Word *code_image, int *ic, const InstructionCode *i_code, InstructionOptions *i_options, char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op, AddressingType *dest_op) {
    /* if the instruction has 2 operands, assume the first is the source and the second is the destination */
    if (line->parameters_count == 2) {
        *raw_src_operand = line->parameters[0];
        *raw_dest_operand = line->parameters[1];
        *src_op = get_operand_addressing_type((*raw_src_operand));
        *dest_op = get_operand_addressing_type((*raw_dest_operand));

    } else if (instruction_has_destination((*i_options))) {
        /* if the instruction has 1 operand and its of type destination, assume there is no source operand */
        *raw_dest_operand = line->parameters[0];
        *src_op = NO_VALUE;
        *dest_op = get_operand_addressing_type((*raw_dest_operand));
    } else if (instruction_has_source((*i_options))) {
        /* if the instruction has 1 operand and its of type source, assume there is no destination operand */
        *raw_src_operand = line->parameters[0];
        *src_op = get_operand_addressing_type((*raw_src_operand));
        *dest_op = NO_VALUE;
    } else {
        /* the instruction has no operands */
        *src_op = NO_VALUE;
        *dest_op = NO_VALUE;
    }
    /* check if the addressing types are legal for the instruction */
    if (!is_addressing_types_legal((*i_options), (*src_op), (*dest_op))) {
        program_log(ERROR,line->line_number, join_strings(2, "invalid operand type for instruction: ", line->main_operand), (char *) line->file_name, TRUE);
        return FALSE;
    }
    /* parse the instruction to a word */
    parse_instruction_to_word(code_image + *ic, (*i_code), (*src_op), (*dest_op));
    return TRUE;
}

void advance_line_operands(ParsedLine *line) {
    int i = 0;
    free(line->main_operand);
    /* make the first parameter the main operand */
    line->main_operand = line->parameters[0];

    for (i = 0; i < line->parameters_count - 1; i++) {
        /* shift the parameters array to the left */
        line->parameters[i] = line->parameters[i + 1];
    }
    /* set the last parameter to NULL */
    line->parameters_count--;
    line->parameters[i] = NULL;
}

InstructionType get_instruction_type(char *instruction) {
    if (is_empty_line(instruction)) return EMPTY_LINE;
    else if (is_comment(instruction)) return COMMENT;
    else if (is_label(instruction)) return LABEL;
    else if (strcmp(instruction, DATA_DIRECTIVE) == 0) return DATA;
    else if (strcmp(instruction, STRING_DIRECTIVE) == 0) return STRING;
    else if (strcmp(instruction, ENTRY_DIRECTIVE) == 0) return ENTRY;
    else if (strcmp(instruction, EXTERN_DIRECTIVE) == 0) return EXTERN;
    else if (get_instruction_code(instruction) != INVALID_INSTRUCTION) return COMMAND;
    else return UNKNOWN;

}

void code_number_into_word_bits(Word *word, int number, int offset, int length) {
    int i = 0;
    /* code the number into the word bits */
    number = number & ((1 << length) - 1);
    /* if the number is negative, convert it to two's complement */
    if (number < 0) {
        number = (1 << length) + number;
    }
    /* code each bit of the number into the word */
    for (i = 0; i < length; i++) {
        word->bits[offset + i] = (number >> i) & 1;
    }
}

void build_instruction_options(int src_is_immediate, int src_is_direct, int src_is_register, int dest_is_immediate, int dest_is_direct, int dest_is_register, InstructionOptions *i_options) {
    /* build the instruction options */
    i_options->src_op.is_register = src_is_register;
    i_options->src_op.is_direct = src_is_direct;
    i_options->src_op.is_immediate = src_is_immediate;
    i_options->dest_op.is_register = dest_is_register;
    i_options->dest_op.is_direct = dest_is_direct;
    i_options->dest_op.is_immediate = dest_is_immediate;
}

void get_instruction_options(InstructionCode i_code, InstructionOptions *i_options) {
    switch (i_code) {
        case MOV:
        case ADD:
        case SUB:
            build_instruction_options(TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, i_options);
            break;
        case CMP:
            build_instruction_options(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, i_options);
            break;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case JSR:
            build_instruction_options(FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, i_options);
            break;
        case LEA:
            build_instruction_options(FALSE, TRUE, FALSE, FALSE, TRUE, TRUE, i_options);
            break;
        case PRN:
            build_instruction_options(FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, i_options);
            break;
        case RTS:
        case STOP:
            build_instruction_options(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, i_options);
            break;
        default:
            build_instruction_options(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, i_options);
    }
}

/*TODO: consts */
void get_word_addressing_types(Word *word, AddressingType *src, AddressingType *dest) {
    /* slots 9-11 are src addressing type */
    *src = (AddressingType) (word->bits[9] + (word->bits[10] << 1) + (word->bits[11] << 2));
    /* slots 2-4 are dest addressing type */
    *dest = (AddressingType) (word->bits[2] + (word->bits[3] << 1) + (word->bits[4] << 2));
}


Bool is_valid_commas(char *line, char *error_message, int non_parameter_tokens_count) {
    int token_count = 0,comma_count = 0,i = 0;
    int len = strlen(line);
    Bool is_previous_char_comma = FALSE,is_token_start = TRUE;

    for (i = 0; i < len; i++) {
        /* skip spaces */
        if (isspace(line[i])) {
            is_token_start = TRUE;
            continue;
        } else if (line[i] == COMMA) {
            /* check for consecutive commas */
            if (is_previous_char_comma) {
                strcpy(error_message, "Multiple consecutive commas found");
                return FALSE;
            }
            /* check for comma before the first parameter */
            if (token_count <= non_parameter_tokens_count) {
                strcpy(error_message, "Invalid comma before the first parameter");
                return FALSE;
            }
            /* update flag */
            is_previous_char_comma = TRUE;
            comma_count++;
        } else if (is_token_start) {
            /* check for missing comma */
            if (token_count > non_parameter_tokens_count) {
                if (!is_previous_char_comma && comma_count <= token_count - non_parameter_tokens_count) {
                    strcpy(error_message, "Comma missing before a parameter token");
                    return FALSE;
                }
            }
            is_previous_char_comma = FALSE;
            token_count++;
            is_token_start = FALSE;
        } else {
            /* update flag for non comma char */
            is_previous_char_comma = FALSE;
        }
    }
    /* check for comma at the end of the line */
    if (is_previous_char_comma) {
        strcpy(error_message, "Invalid comma found at the end of the line");
        return FALSE;
    }
    return TRUE;
}