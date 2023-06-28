
#include "error.h"
#include <malloc.h>
#include <string.h>
#include "instruction_handling.h"
#include "utils.h"


Bool handle_command_operand(const OperandRow *row,  Word *code_image, int *ic, Opcode *op_num, OpcodeMode *op_mode,
                            char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op,
                            AddressingType *dest_op);


Bool
handle_parameter_operands(OperandRow *row, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand,
                          AddressingType src_op, AddressingType dest_op);

void parse_registers_to_word(Word *word, Register src_register, Register dest_register);

void parse_operand_to_word(Word *word, Opcode opcode, AddressingType src_op, AddressingType dest_op);

AddressingType get_addressing_type(char *operand);

void parse_int_to_word(Word *word, int num, Bool add_ARE);

Bool code_word_from_operand(OperandRow *row, Word *code_image, int *ic, char *raw_operand,
                            AddressingType addressing_type, OperandLocation location);

void empty_word(Word *word);

void empty_word(Word *word) {
    int i;
    for (i = 0; i < WORD_SIZE; i++) {
        word->bits[i] = FALSE;
    }
}

void parse_registers_to_word(Word *word, Register src_register, Register dest_register) {
    empty_word(word);
    code_number_into_word_bits(word, ABSOLUTE, 0, 2);
    code_number_into_word_bits(word, dest_register, 2, 5);
    code_number_into_word_bits(word, src_register, 7, 5);
}

void parse_operand_to_word(Word *word, Opcode opcode, AddressingType src_op, AddressingType dest_op) {
    empty_word(word);
    code_number_into_word_bits(word, ABSOLUTE, 0, 2);
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
        code_number_into_word_bits(word, ABSOLUTE, 0, 2);
        code_number_into_word_bits(word, num, 2, 10);
    } else {
        code_number_into_word_bits(word, num, 0, 12);
    }
}

Bool address_data_instruction(OperandRow *row, Word *data_image, int *dc) {
    /* TODO: valdiate original row is correctly formatted */
    int i;
    for (i = 0; i < row->parameters_count; i++) {
        parse_int_to_word(&(data_image[*dc]), parse_int(row->parameters[i]), FALSE);
        (*dc)++;
    }
    return TRUE;
}


Bool address_string_instruction(OperandRow *row, Word *data_image, int *dc) {
    int i, j;
    /* TODO: valdiate original row is correctly formatted */
    for (i = 0; i < strlen(row->parameters[0]); i++) {
        /* TODO: I doubt this works, need to check int parse */
        parse_int_to_word(&(data_image[*dc]), (int) (row->parameters[0][i]), FALSE);
        (*dc)++;
    }
    return TRUE;
}

Bool code_word_from_operand(OperandRow *row, Word *code_image, int *ic, char *raw_operand,
                            AddressingType const addressing_type, OperandLocation const location) {
    if (addressing_type == IMMEDIATE) {
        parse_int_to_word(code_image + *ic, parse_int(raw_operand), TRUE);
        (*ic)++;
    } else if (addressing_type == DIRECT) {
        empty_word(&code_image[*ic]);
        (*ic)++;
    } else {
        Register reg = get_register(raw_operand);
        if (reg == INVALID_REGISTER) {
            /* TODO: error */
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

Bool address_code_instruction(OperandRow *row, Word *code_image, int *ic) {
    /* TODO: valdiate original row is correctly formatted */
    char *raw_src_operand, *raw_dest_operand;
    AddressingType src_op, dest_op;

    Opcode op_num = get_opcode(row->operand);
    if (op_num == INVALID_OPCODE) {
        /* TODO: thrwo error */
        return FALSE;
    }
    OpcodeMode op_mode = get_opcode_possible_modes(op_num);
    if (!handle_command_operand(row, code_image, ic, &op_num, &op_mode, &raw_src_operand, &raw_dest_operand, &src_op,
                                &dest_op)) {
        return FALSE;
    };
    if (row->parameters_count == 0) {
        return TRUE;
    }
    if (!handle_parameter_operands(row, code_image, ic, raw_src_operand, raw_dest_operand, src_op, dest_op)) {
        return FALSE;
    }


    return TRUE;
}



Bool handle_parameter_operands(OperandRow *row, Word *code_image, int *ic, char *raw_src_operand, char *raw_dest_operand,
                          AddressingType const src_op, AddressingType const dest_op) {
    Register src_register, dest_register;
    Bool is_success = TRUE;
    if (row->parameters_count == 1) {
        if ((src_op) == NO_VALUE) {
            is_success = code_word_from_operand(row, code_image, ic, raw_dest_operand, dest_op, DESTINATION);
        } else {
            is_success = code_word_from_operand(row, code_image, ic, raw_src_operand, src_op, SOURCE);
        }
    } else {
        is_success = code_word_from_operand(row, code_image, ic, raw_src_operand, src_op, SOURCE);
        if (!((src_op) == REGISTER && dest_op == REGISTER)) {
            is_success = code_word_from_operand(row, code_image, ic, raw_dest_operand, dest_op, DESTINATION);
        } else {
            src_register = get_register(raw_src_operand);
            dest_register = get_register(raw_dest_operand);
            if (src_register == INVALID_REGISTER || dest_register == INVALID_REGISTER) {
                /* TODO: throw error */
                return FALSE;
            }
            parse_registers_to_word(&(code_image[*ic - 1]), src_register, dest_register);
        }
    }
    return is_success;
}

Bool handle_command_operand(const OperandRow *row,  Word *code_image, int *ic, Opcode *op_num, OpcodeMode *op_mode,
                            char **raw_src_operand, char **raw_dest_operand, AddressingType *src_op,
                            AddressingType *dest_op) {
    if (row->parameters_count == 2) {
        (*raw_src_operand) = row->parameters[0];
        (*raw_dest_operand) = row->parameters[1];
        /*TODO: check if op_num is valid */
        (*src_op) = get_addressing_type((*raw_src_operand));
        (*dest_op) = get_addressing_type((*raw_dest_operand));

    } else if (opcode_has_destination((*op_mode))) {
        (*raw_dest_operand) = row->parameters[0];
        (*src_op) = NO_VALUE;
        (*dest_op) = get_addressing_type((*raw_dest_operand));
    } else if (opcode_has_source((*op_mode))) {
        (*raw_src_operand) = row->parameters[0];
        (*src_op) = get_addressing_type((*raw_src_operand));
        (*dest_op) = NO_VALUE;
    } else {
        (*src_op) = NO_VALUE;
        (*dest_op) = NO_VALUE;
    }
    parse_operand_to_word(code_image + *ic, (*op_num), (*src_op), (*dest_op));
    (*ic)++;
    return TRUE;
}


InstructionType get_instruction_type(char *instruction) {
    if (is_empty_row(instruction)) {
        return EMPTY_ROW;
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

void code_number_into_word_bits(Word *word, int number, int offset, int length){
    int i = 0;
    number = number & ((1 << length) - 1);
    if(number < 0){
        number = (1 << length) + number;
    }
    for(i = 0; i < length; i++){
        word->bits[offset + i] = (number >> i) & 1;
    }
}

OpcodeMode get_opcode_possible_modes(Opcode opcode){
    OpcodeMode opcode_mode = {0};
    ParameterMode src_mode = {0};
    ParameterMode dest_mode = {0};
    opcode_mode.src_op = src_mode;
    opcode_mode.dest_op = dest_mode;
    switch (opcode){
        case MOV:
        case ADD:
        case SUB:
            build_opcode_mode(TRUE,TRUE,TRUE,FALSE,TRUE,TRUE,&opcode_mode);
            break;
        case CMP:
            build_opcode_mode(TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,&opcode_mode);
            break;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case JSR:
            build_opcode_mode(FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,&opcode_mode);
            break;
        case LEA:
            build_opcode_mode(FALSE,TRUE,FALSE,FALSE,TRUE,TRUE,&opcode_mode);
            break;
        case PRN:
            build_opcode_mode(TRUE,TRUE,TRUE,FALSE,FALSE,FALSE,&opcode_mode);
            break;
        case RTS:
        case STOP:
            build_opcode_mode(FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,&opcode_mode);
            break;
        default:
            build_opcode_mode(FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,&opcode_mode);
    }
    return opcode_mode;
}