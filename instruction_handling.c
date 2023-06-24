
#include <string.h>
#include "instruction_handling.h"


void empty_word(Word *word) {
    int i;
    for (i = 0; i < WORD_SIZE; i++) {
        word->bits[i] = FALSE;
    }
}

void code_number_into_word_bits(Word *word, int number,int offset, int length){
    int i = 0;
    Bool is_negative = FALSE;
    if(number < 0){
        is_negative = TRUE;
        number = -number;
    }
    for(i = 0; i < length; i++){
        word->bits[offset + i] = (number >> i) & 1;
    }
    if(is_negative){
        for(i = 0; i < length; i++){
            word->bits[offset + i] = !word->bits[offset + i];
        }
        word->bits[offset] = TRUE;
    }
}

/* CHeck error */
Register get_register(char *operand){
    return (Register) (operand[2] - '0');
}

void parse_registers_to_word(Word *word, Register src_register, Register dest_register) {
    empty_word(word);
    code_number_into_word_bits(word, ABSOLUTE, 0, 2);
    code_number_into_word_bits(word, dest_register, 2, 5);
    code_number_into_word_bits(word, src_register, 7, 5);
}

void parse_operand_to_word(Word *word, Opcode opcode,  AddressingType src_op, AddressingType dest_op) {
    empty_word(word);
    code_number_into_word_bits(word, ABSOLUTE, 0, 2);
    code_number_into_word_bits(word, dest_op, 2, 3);
    code_number_into_word_bits(word, opcode, 5, 4);
    code_number_into_word_bits(word, src_op, 9, 3);
}

Bool is_register(char *operand) {
    if (operand[0] == '@' ) {
        return TRUE;
    }
    return FALSE;
}

AddressingType get_addressing_type(char *operand) {
    if(is_integer(operand)) {
        return IMMEDIATE;
    }else if(is_register(operand)) {
        return REGISTER;
    }else {
        return DIRECT;
    }
}
 /* TODO: clean this */
Opcode get_opcode(char *command) {
    if (strcmp(command, MOV_COMMAND) == 0) {
        return MOV;
    } else if (strcmp(command, CMP_COMMAND) == 0) {
        return CMP;
    } else if (strcmp(command, ADD_COMMAND) == 0) {
        return ADD;
    } else if (strcmp(command, SUB_COMMAND) == 0) {
        return SUB;
    } else if (strcmp(command, NOT_COMMAND) == 0) {
        return NOT;
    } else if (strcmp(command, CLR_COMMAND) == 0) {
        return CLR;
    } else if (strcmp(command, LEA_COMMAND) == 0) {
        return LEA;
    } else if (strcmp(command, INC_COMMAND) == 0) {
        return INC;
    } else if (strcmp(command, DEC_COMMAND) == 0) {
        return DEC;
    }else if (strcmp(command,JMP_COMMAND) == 0)
        return JMP;
    else if (strcmp(command,BNE_COMMAND) == 0)
        return BNE;
    else if (strcmp(command,RED_COMMAND) == 0)
        return RED;
    else if (strcmp(command,PRN_COMMAND) == 0)
        return PRN;
    else if (strcmp(command,JSR_COMMAND) == 0)
        return JSR;
    else if (strcmp(command,RTS_COMMAND) == 0)
        return RTS;
    else if (strcmp(command,STOP_COMMAND) == 0)
        return STOP;
    return INVALID_OPCODE;
}
/* TODO: make this work with negative numbers
 * TODO: make this work with size check*/
void parse_int_to_word(Word *word,int num,Bool add_ARE) {
    if(add_ARE){
        code_number_into_word_bits(word, ABSOLUTE, 0, 2);
        code_number_into_word_bits(word, num, 2, 10);
    }else{
        code_number_into_word_bits(word, num, 0, 12);
    }
}

Bool address_data_instruction(OperandRow *row, Word *data_image, int *dc) {
    /* TODO: valdiate original row is correctly formatted */
    int i;
    for (i = 0; i < row->parameters_count; i++) {
        /* TODO: I doubt this works, need to check int parse */
         parse_int_to_word(&(data_image[*dc]),parse_int(row->parameters[i]),FALSE);
        (*dc)++;
    }
    return TRUE;
}

Bool address_string_instruction(OperandRow *row, Word *data_image, int *dc) {
    int i, j;
    /* TODO: valdiate original row is correctly formatted */
    for (i = 0; i < strlen(row->parameters[0]); i++) {
        /* TODO: I doubt this works, need to check int parse */
        parse_int_to_word(&(data_image[*dc]),(int) (row->parameters[0][i]),FALSE);
        (*dc)++;
    }
    return TRUE;
}

void handle_operand(OperandRow *row, Word *code_image, int *ic,char *raw_operand,AddressingType addressing_type,OperandLocation location){
    if (addressing_type == IMMEDIATE){
        parse_int_to_word(code_image + *ic,parse_int(raw_operand),TRUE);
        (*ic)++;
    }else if(addressing_type == DIRECT){
        empty_word(&code_image[*ic]);
        (*ic)++;
    }else{
        Register reg = get_register(raw_operand);
        if(location == DESTINATION) {
            parse_registers_to_word(&(code_image[*ic]), 0, reg);
        }else{
            parse_registers_to_word(&(code_image[*ic]), reg, 0);
        }
        (*ic)++;
    }
}

Bool address_code_instruction(OperandRow *row, Word *code_image, int *ic) {
    /* TODO: valdiate original row is correctly formatted */
    Register src_register, dest_register;
    Opcode op_num = get_opcode(row->operand);
    AddressingType src_op , dest_op;
    char* raw_src_operand = row->parameters_count ? row->parameters[0] : NULL;
    char* raw_dest_operand = row->parameters_count > 1 ? row->parameters[1] : NULL;
    /*TODO: check if op_num is valid */
    src_op = row->parameters_count? get_addressing_type(row->parameters[0]): NO_ADD_VALUE;
    dest_op = row->parameters_count >1? get_addressing_type(row->parameters[1]): NO_ADD_VALUE;
    parse_operand_to_word(code_image + *ic, op_num,src_op,dest_op);
    (*ic)++;
    if(!(row->parameters_count)) {
        return TRUE;
    }
    handle_operand(row,code_image,ic,raw_src_operand,src_op,SOURCE);
    if(row->parameters_count == 1) {
        return TRUE;
    }
    if(!(src_op == REGISTER && dest_op == REGISTER)){
        handle_operand(row,code_image,ic,raw_dest_operand,dest_op,DESTINATION);
    }else{
        src_register = get_register(raw_src_operand);
        dest_register = get_register(raw_dest_operand);
        parse_registers_to_word(&(code_image[*ic - 1]),src_register,dest_register);
    }
    return TRUE;
}

Bool is_label(char *instruction) {
    return instruction[strlen(instruction) - 1] == LABEL_TERMINATOR;
}


InstructionType get_instruction_type(char *instruction) {
    if (is_comment(instruction)) {
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

int parse_int(char *str) {
    int num;
    sscanf(str, "%d", &num);
    return num;
}
