
#include "error.h"
#include <malloc.h>
#include <string.h>
#include "instruction_handling.h"
#include "utils.h"


Bool handle_instruction_operand(const ParsedLine *line, Word *code_image, int *ic, Opcode *op_num, OpcodeMode *op_mode,
                                char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op,
                                AddressingType *dest_op);


Bool
handle_parameter_operands(ParsedLine *line, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand,
                          AddressingType src_op, AddressingType dest_op);

void parse_registers_to_word(Word *word, Register src_register, Register dest_register);

void parse_operand_to_word(Word *word, Opcode opcode, AddressingType src_op, AddressingType dest_op);

AddressingType get_addressing_type(char *operand);

void parse_int_to_word(Word *word, int num, Bool add_ARE);

Bool code_word_from_operand(ParsedLine *line, Word *code_image, int *ic, char *raw_operand,
                            AddressingType addressing_type, OperandLocation location);

void set_word_to_zero(Word *word);

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
    code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, 0, 2);
    code_number_into_word_bits(word, dest_register, 2, 5);
    code_number_into_word_bits(word, src_register, 7, 5);
}

void parse_operand_to_word(Word *word, Opcode opcode, AddressingType src_op, AddressingType dest_op) {
    set_word_to_zero(word);
    code_number_into_word_bits(word, ABSOLUTE_ADDRESSING, 0, 2);
    code_number_into_word_bits(word, dest_op, 2, 3);
    code_number_into_word_bits(word, opcode, 5, 4);
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
    /* TODO: valdiate original line is correctly formatted */
    int i;
    for (i = 0; i < line->parameters_count; i++) {
        parse_int_to_word(&(data_image[*dc]), parse_int(line->parameters[i]), FALSE);
        (*dc)++;
    }
    return TRUE;
}

Bool is_legal_string_operand(ParsedLine *line) {
    int i;
    char *string = line->parameters[0];
    if (string[0] != STRING_BOUNDARY || string[strlen(string) - 1] != STRING_BOUNDARY) {
        throw_program_error(line->line_number, "string must be enclosed in double quotes", line->file_name, FALSE);
        return FALSE;
    }
    for (i = 0; i < strlen(string); i++) {
        if ((string[i] & ~ASCII_MAX) != 0) {
            throw_program_error(line->line_number, "string contains non-ascii characters", line->file_name, FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

Bool address_string_instruction(ParsedLine *line, Word *data_image, int *dc) {
    int i;
    char *string_token = line->parameters[0];
    /* TODO: valdiate original line is correctly formatted */
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

Bool code_word_from_operand(ParsedLine *line, Word *code_image, int *ic, char *raw_operand, AddressingType const addressing_type, OperandLocation const location) {
    Register reg;
    if (addressing_type == IMMEDIATE) {
        parse_int_to_word(code_image + *ic, parse_int(raw_operand), TRUE);
        (*ic)++;
    } else if (addressing_type == DIRECT) {
        set_word_to_zero(&code_image[*ic]);
        (*ic)++;
    } else {
        reg = get_register(raw_operand);
        if (reg == INVALID_REGISTER) {
            throw_program_error(line->line_number, join_strings(2, "invalid register: ", raw_operand), line->file_name, TRUE);
            return FALSE;
        }
        if (location == DESTINATION) {
            parse_registers_to_word(&(code_image[*ic]), 0, reg);
        } else {
            parse_registers_to_word(&(code_image[*ic]), reg, 0);
        }
        (*ic)++;
    }
    return TRUE;
}

Bool opcode_has_source(OpcodeMode opcode_mode) {
    return opcode_mode.src_op.is_direct || opcode_mode.src_op.is_immediate || opcode_mode.src_op.is_register;
}

Bool opcode_has_destination(OpcodeMode opcode_mode) {
    return opcode_mode.dest_op.is_direct || opcode_mode.dest_op.is_immediate || opcode_mode.dest_op.is_register;
}

Bool address_code_instruction(ParsedLine *line, Word *code_image, int *ic) {
    /* TODO:  valdiate original line is correctly formatted */
    char *raw_src_operand = NULL, *raw_dest_operand = NULL;
    AddressingType src_op = {0}, dest_op = {0};
    OpcodeMode op_mode = {0};

    Opcode op_num = get_opcode(line->operand);
    if (op_num == INVALID_OPCODE) {
        throw_program_error(line->line_number, join_strings(2, "invalid instruction: ", line->operand), line->file_name, TRUE);
        return FALSE;
    }
    op_mode = get_opcode_possible_modes(op_num);
    if (line->parameters_count != opcode_has_source(op_mode) + opcode_has_destination(op_mode)) {
        throw_program_error(line->line_number, join_strings(2, "invalid number of operands for instruction: ", line->operand), line->file_name, TRUE);
        return FALSE;
    }
    if (!handle_instruction_operand(line, code_image, ic, &op_num, &op_mode, &raw_src_operand, &raw_dest_operand, &src_op, &dest_op)) return FALSE;
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
    } else {
        if (CHECK_AND_UPDATE_SUCCESS(is_success, code_word_from_operand(line, code_image, ic, raw_src_operand, src_op, SOURCE))) {
            if (!((src_op) == REGISTER && dest_op == REGISTER)) {
                CHECK_AND_UPDATE_SUCCESS(is_success, code_word_from_operand(line, code_image, ic, raw_dest_operand, dest_op, DESTINATION));
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

Bool is_addressing_types_legal(OpcodeMode const opcode_mode, AddressingType const src_op, AddressingType const dest_op) {
    if (!opcode_mode.src_op.is_direct && src_op == DIRECT) return FALSE;
    if (!opcode_mode.src_op.is_immediate && src_op == IMMEDIATE) return FALSE;
    if (!opcode_mode.src_op.is_register && src_op == REGISTER) return FALSE;
    if (!opcode_mode.dest_op.is_direct && dest_op == DIRECT) return FALSE;
    if (!opcode_mode.dest_op.is_immediate && dest_op == IMMEDIATE) return FALSE;
    if (!opcode_mode.dest_op.is_register && dest_op == REGISTER) return FALSE;
    return TRUE;
}

Bool handle_instruction_operand(const ParsedLine *line, Word *code_image, int *ic, Opcode *op_num, OpcodeMode *op_mode, char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op, AddressingType *dest_op) {
    if (line->parameters_count == 2) {
        *raw_src_operand = line->parameters[0];
        *raw_dest_operand = line->parameters[1];
        *src_op = get_addressing_type((*raw_src_operand));
        *dest_op = get_addressing_type((*raw_dest_operand));

    } else if (opcode_has_destination((*op_mode))) {
        *raw_dest_operand = line->parameters[0];
        *src_op = NO_VALUE;
        *dest_op = get_addressing_type((*raw_dest_operand));
    } else if (opcode_has_source((*op_mode))) {
        *raw_src_operand = line->parameters[0];
        *src_op = get_addressing_type((*raw_src_operand));
        *dest_op = NO_VALUE;
    } else {
        *src_op = NO_VALUE;
        *dest_op = NO_VALUE;
    }
    if (!is_addressing_types_legal((*op_mode), (*src_op), (*dest_op))) {
        throw_program_error(line->line_number, join_strings(2, "invalid operand type for instruction: ", line->operand), (char *) line->file_name, TRUE);
        return FALSE;
    }
    parse_operand_to_word(code_image + *ic, (*op_num), (*src_op), (*dest_op));
    (*ic)++;
    return TRUE;
}

void advance_line_operands(ParsedLine *line) {
    int i = 0;
    line->operand = line->parameters[0];
    for (i = 0; i < line->parameters_count - 1; i++) {
        line->parameters[i] = line->parameters[i + 1];
    }
    line->parameters_count--;
    line->parameters[i] = NULL;
}

InstructionType get_instruction_type(char *instruction) {
    if (is_empty_line(instruction)) {
        return EMPTY_LINE;
    } else if (is_comment(instruction)) {
        return COMMENT;
    } else if (is_label(instruction)) {
        return LABEL;
    } else if (strcmp(instruction, DATA_DIRECTIVE) == 0) {
        return DATA;
    } else if (strcmp(instruction, STRING_DIRECTIVE) == 0) {
        return STRING;
    } else if (strcmp(instruction, ENTRY_DIRECTIVE) == 0) {
        return ENTRY;
    } else if (strcmp(instruction, EXTERN_DIRECTIVE) == 0) {
        return EXTERN;
    } else if (get_opcode(instruction) != INVALID_OPCODE) {
        return COMMAND;
    } else {
        return UNKNOWN;
    }
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

void build_opcode_mode(int src_is_immediate, int src_is_direct, int src_is_register, int dest_is_immediate, int dest_is_direct, int dest_is_register, OpcodeMode *opcode_mode) {
    opcode_mode->src_op.is_register = src_is_register;
    opcode_mode->src_op.is_direct = src_is_direct;
    opcode_mode->src_op.is_immediate = src_is_immediate;
    opcode_mode->dest_op.is_register = dest_is_register;
    opcode_mode->dest_op.is_direct = dest_is_direct;
    opcode_mode->dest_op.is_immediate = dest_is_immediate;
}

OpcodeMode get_opcode_possible_modes(Opcode opcode) {
    OpcodeMode opcode_mode = {0};
    ParameterMode src_mode = {0};
    ParameterMode dest_mode = {0};
    opcode_mode.src_op = src_mode;
    opcode_mode.dest_op = dest_mode;
    switch (opcode) {
        case MOV:
        case ADD:
        case SUB:
            build_opcode_mode(TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, &opcode_mode);
            break;
        case CMP:
            build_opcode_mode(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, &opcode_mode);
            break;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case JSR:
            build_opcode_mode(FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, &opcode_mode);
            break;
        case LEA:
            build_opcode_mode(FALSE, TRUE, FALSE, FALSE, TRUE, TRUE, &opcode_mode);
            break;
        case PRN:
            build_opcode_mode(FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, &opcode_mode);
            break;
        case RTS:
        case STOP:
            build_opcode_mode(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, &opcode_mode);
            break;
        default:
            build_opcode_mode(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, &opcode_mode);
    }
    return opcode_mode;
}

void get_word_addressing_types(Word *word, AddressingType *src, AddressingType *dest) {
    *src = (AddressingType) (word->bits[9] + (word->bits[10] << 1) + (word->bits[11] << 2));
    *dest = (AddressingType) (word->bits[2] + (word->bits[3] << 1) + (word->bits[4] << 2));
}