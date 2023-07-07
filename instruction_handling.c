
#include "error.h"
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "instruction_handling.h"
#include "utils.h"

static Bool instruction_has_destination(InstructionOptions i_options);

static Bool instruction_has_source(InstructionOptions i_options);

static Bool is_addressing_types_legal(InstructionOptions i_options, AddressingType src_op, AddressingType dest_op);

static void build_instruction_options(int src_is_immediate, int src_is_direct, int src_is_register, int dest_is_immediate, int dest_is_direct, int dest_is_register, InstructionOptions *i_options);

static Bool handle_instruction(const ParsedLine *line, Word *code_image, int *ic, const InstructionCode *i_code, InstructionOptions *i_options, char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op, AddressingType *dest_op);

static Bool handle_parameter_operands(ParsedLine *line, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand, AddressingType src_op, AddressingType dest_op);

static void parse_registers_to_word(Word *word, Register src_register, Register dest_register);

static void parse_operand_to_word(Word *word, InstructionCode i_code, AddressingType src_op, AddressingType dest_op);

static AddressingType get_addressing_type(char *operand);

static void parse_int_to_word(Word *word, int num, Bool add_ARE);

static Bool code_word_from_operand(ParsedLine *line, Word *code_image, const int *ic, char *raw_operand, AddressingType addressing_type, OperandContext context);

static void set_word_to_zero(Word *word);

static void code_number_into_word_bits(Word *word, int number, int offset, int length);

static void get_instruction_options(InstructionCode i_code, InstructionOptions *i_options);

static Bool is_legal_string_operand(ParsedLine *line);

void set_word_to_zero(Word *word) {
    int i;
    for (i = 0; i < WORD_SIZE; i++) {
        word->bits[i] = FALSE;
    }
}

void parse_symbol_to_word(int symbol_index, Word *word, AddressingMethod addressing_method) {
    code_number_into_word_bits(word, addressing_method, 0, 2);
    code_number_into_word_bits(word, symbol_index, 2, 10);
}

void parse_registers_to_word(Word *word, Register src_register, Register dest_register) {
    set_word_to_zero(word);
    code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, ARE_START_INDEX, ARE_END_INDEX);
    code_number_into_word_bits(word, dest_register, 2, 5);
    code_number_into_word_bits(word, src_register, 7, 5);
}

void parse_operand_to_word(Word *word, InstructionCode i_code, AddressingType src_op, AddressingType dest_op) {
    set_word_to_zero(word);
    code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, ARE_START_INDEX, ARE_END_INDEX);
    code_number_into_word_bits(word, dest_op, 2, 3);
    code_number_into_word_bits(word, i_code, 5, 4);
    code_number_into_word_bits(word, src_op, 9, 3);
}

AddressingType get_addressing_type(char *operand) {
    if (is_integer(operand)) {
        return IMMEDIATE;
    } else if (is_register(operand)) {
        return REGISTER;
    } else {
        return DIRECT;
    }
}

/* TODO: make this work with size check*/
void parse_int_to_word(Word *word, int num, Bool add_ARE) {
    if (add_ARE) {
        code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, 0, 2);
        code_number_into_word_bits(word, num, 2, 10);
    } else {
        code_number_into_word_bits(word, num, 0, 12);
    }
}

Bool address_data_instruction(ParsedLine *line, Word *data_image, int *dc) {
    int i;
    for (i = 0; i < line->parameters_count; i++) {
        parse_int_to_word(&(data_image[*dc]), parse_int(line->parameters[i]), FALSE);
        (*dc)++;
    }
    return TRUE;
}

Bool is_legal_string_operand(ParsedLine *line) {
    int i;
    char *first_string = line->parameters[0];
    char *last_string = line->parameters[line->parameters_count - 1];
    if (first_string[0] != STRING_BOUNDARY || last_string[strlen(last_string) - 1] != STRING_BOUNDARY) {
        throw_program_error(line->line_number, "first_string must be enclosed in double quotes", line->file_name, FALSE);
        return FALSE;
    }
    for (i = 0; i < strlen(first_string); i++) {
        if ((first_string[i] & ~ASCII_MAX) != 0) {
            throw_program_error(line->line_number, "first_string contains non-ascii characters", line->file_name, FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

Bool address_string_instruction(ParsedLine *line, Word *data_image, int *dc) {
    int i;
    char *string_token = line->parameters[0];
    if (!is_legal_string_operand(line)) return FALSE;
    trim_string_quotes(string_token);
    for (i = 0; i < strlen(string_token); i++) {
        parse_int_to_word(&(data_image[*dc]), (int) (string_token[i]), FALSE);
        (*dc)++;
    }
    parse_int_to_word(&(data_image[*dc]), (int) (NULL_CHAR), FALSE);
    (*dc)++;
    return TRUE;
}

Bool code_word_from_operand(ParsedLine *line, Word *code_image,const int *ic, char *raw_operand, AddressingType const addressing_type, OperandContext const context) {
    Register reg;
    if (addressing_type == IMMEDIATE) {
        parse_int_to_word(code_image + *ic, parse_int(raw_operand), TRUE);
    } else if (addressing_type == DIRECT) {
        set_word_to_zero(&code_image[*ic]);
    } else {
        reg = get_register(raw_operand);
        if (reg == INVALID_REGISTER) {
            throw_program_error(line->line_number, join_strings(2, "invalid register: ", raw_operand), line->file_name, TRUE);
            return FALSE;
        }
        if (context == DESTINATION) {
            parse_registers_to_word(&(code_image[*ic]), 0, reg);
        } else {
            parse_registers_to_word(&(code_image[*ic]), reg, 0);
        }
    }
    return TRUE;
}

Bool instruction_has_source(InstructionOptions i_options) {
    return i_options.src_op.is_direct || i_options.src_op.is_immediate || i_options.src_op.is_register;
}

Bool instruction_has_destination(InstructionOptions i_options) {
    return i_options.dest_op.is_direct || i_options.dest_op.is_immediate || i_options.dest_op.is_register;
}

Bool address_code_instruction(ParsedLine *line, Word *code_image, int *ic) {
    char *raw_src_operand = NULL, *raw_dest_operand = NULL;
    AddressingType src_op = {0}, dest_op = {0};
    InstructionOptions i_options = {0};
    ParameterMode src_mode = {0}, dest_mode = {0};
    i_options.src_op = src_mode;
    i_options.dest_op = dest_mode;


    InstructionCode op_num = get_instruction_code(line->main_operand);
    if (op_num == INVALID_INSTRUCTION) {
        throw_program_error(line->line_number, join_strings(2, "invalid instruction: ", line->main_operand), line->file_name, TRUE);
        return FALSE;
    }
    get_instruction_options(op_num, &i_options);
    if (line->parameters_count != instruction_has_source(i_options) + instruction_has_destination(i_options)) {
        throw_program_error(line->line_number, join_strings(2, "invalid number of operands for instruction: ", line->main_operand), line->file_name, TRUE);
        return FALSE;
    }
    if (!handle_instruction(line, code_image, ic, &op_num, &i_options, &raw_src_operand, &raw_dest_operand, &src_op, &dest_op)) return FALSE;
    if (line->parameters_count == 0) return TRUE;
    if (!handle_parameter_operands(line, code_image, ic, raw_src_operand, raw_dest_operand, src_op, dest_op)) return FALSE;

    return TRUE;
}

Bool handle_parameter_operands(ParsedLine *line, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand, AddressingType const src_op, AddressingType const dest_op) {
    Register src_register, dest_register;
    Bool is_success = TRUE;
    if (line->parameters_count == 1) {
        if ((src_op) == NO_VALUE) {
            CHECK_AND_UPDATE_SUCCESS(is_success, code_word_from_operand(line, code_image, ic, raw_dest_operand, dest_op, DESTINATION));
        } else {
            CHECK_AND_UPDATE_SUCCESS(is_success, code_word_from_operand(line, code_image, ic, raw_src_operand, src_op, SOURCE));
        }
        (*ic)++;
    } else {
        if (CHECK_AND_UPDATE_SUCCESS(is_success, code_word_from_operand(line, code_image, ic, raw_src_operand, src_op, SOURCE))) {
            (*ic)++;
            if (!(src_op == REGISTER && dest_op == REGISTER)) {
                CHECK_AND_UPDATE_SUCCESS(is_success, code_word_from_operand(line, code_image, ic, raw_dest_operand, dest_op, DESTINATION));
                (*ic)++;
            } else {
                src_register = get_register(raw_src_operand);
                dest_register = get_register(raw_dest_operand);
                if (src_register == INVALID_REGISTER || dest_register == INVALID_REGISTER) {
                    if (src_register == INVALID_REGISTER) {
                        throw_program_error(line->line_number, join_strings(2, "invalid register: ", raw_src_operand), line->file_name, TRUE);
                    }
                    if (dest_register == INVALID_REGISTER) {
                        throw_program_error(line->line_number, join_strings(2, "invalid register: ", raw_dest_operand), line->file_name, TRUE);
                    }
                    return FALSE;
                }
                parse_registers_to_word(&(code_image[*ic - 1]), src_register, dest_register);
            }
        }
    }
    return is_success;
}

Bool is_addressing_types_legal(InstructionOptions const i_options, AddressingType const src_op, AddressingType const dest_op) {
    if (!i_options.src_op.is_direct && src_op == DIRECT) return FALSE;
    if (!i_options.src_op.is_immediate && src_op == IMMEDIATE) return FALSE;
    if (!i_options.src_op.is_register && src_op == REGISTER) return FALSE;
    if (!i_options.dest_op.is_direct && dest_op == DIRECT) return FALSE;
    if (!i_options.dest_op.is_immediate && dest_op == IMMEDIATE) return FALSE;
    if (!i_options.dest_op.is_register && dest_op == REGISTER) return FALSE;
    return TRUE;
}

Bool handle_instruction(const ParsedLine *line, Word *code_image, int *ic, const InstructionCode *i_code, InstructionOptions *i_options, char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op, AddressingType *dest_op) {
    if (line->parameters_count == 2) {
        *raw_src_operand = line->parameters[0];
        *raw_dest_operand = line->parameters[1];
        *src_op = get_addressing_type((*raw_src_operand));
        *dest_op = get_addressing_type((*raw_dest_operand));

    } else if (instruction_has_destination((*i_options))) {
        *raw_dest_operand = line->parameters[0];
        *src_op = NO_VALUE;
        *dest_op = get_addressing_type((*raw_dest_operand));
    } else if (instruction_has_source((*i_options))) {
        *raw_src_operand = line->parameters[0];
        *src_op = get_addressing_type((*raw_src_operand));
        *dest_op = NO_VALUE;
    } else {
        *src_op = NO_VALUE;
        *dest_op = NO_VALUE;
    }
    if (!is_addressing_types_legal((*i_options), (*src_op), (*dest_op))) {
        throw_program_error(line->line_number, join_strings(2, "invalid main_operand type for instruction: ", line->main_operand), (char *) line->file_name, TRUE);
        return FALSE;
    }
    parse_operand_to_word(code_image + *ic, (*i_code), (*src_op), (*dest_op));
    (*ic)++;
    return TRUE;
}

void advance_line_operands(ParsedLine *line) {
    int i = 0;
    free(line->main_operand);
    line->main_operand = line->parameters[0];
    for (i = 0; i < line->parameters_count - 1; i++) {
        line->parameters[i] = line->parameters[i + 1];
    }
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
    number = number & ((1 << length) - 1);
    if (number < 0) {
        number = (1 << length) + number;
    }
    for (i = 0; i < length; i++) {
        word->bits[offset + i] = (number >> i) & 1;
    }
}

void build_instruction_options(int src_is_immediate, int src_is_direct, int src_is_register, int dest_is_immediate, int dest_is_direct, int dest_is_register, InstructionOptions *i_options) {
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

void get_word_addressing_types(Word *word, AddressingType *src, AddressingType *dest) {
    *src = (AddressingType) (word->bits[9] + (word->bits[10] << 1) + (word->bits[11] << 2));
    *dest = (AddressingType) (word->bits[2] + (word->bits[3] << 1) + (word->bits[4] << 2));
}

Bool is_valid_commas(char *line, char *error_message) {
    int len = strlen(line);
    int token_count = 0, i = 0;
    Bool is_previous_char_comma = FALSE;

    for (i = 0; i < len; i++) {
        while (!isspace(line[i]) && line[i] != ',') {
            is_previous_char_comma = FALSE;
            if(i<len-1){
                i++;
            } else {
                break;
            }
        };
        if (i> 0 && line[i - 1] != ':') {
            token_count++;
        }
        if (isspace(line[i])) continue;
        if (line[i] == ',') {
            if (token_count <= 1) {
                strcpy(error_message, "misplaced comma before the first parameter");
                return FALSE;
            };
            if (is_previous_char_comma) {
                strcpy(error_message, "multiple consecutive commas");
                return FALSE;
            };
            is_previous_char_comma = TRUE;
        } else {
            is_previous_char_comma = FALSE;
        }
    }
    if(is_previous_char_comma) {
        strcpy(error_message, "misplaced comma after the last parameter");
        return FALSE;
    }
    return TRUE;
}