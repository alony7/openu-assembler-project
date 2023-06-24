#include <malloc.h>
#include <string.h>
#include "utils.h"
#include "instruction_handling.h"

char *string_array_to_string(char **array, int size){
    int i;
    char *string = malloc(sizeof(char));
    string[0] = '\0';
    for(i=0;i<size;i++){
        string = realloc(string, (strlen(string) + strlen(array[i]) + 1) * sizeof(char));
        strcat(string, array[i]);
    }
    return string;
}

char* duplicate_string(const char* str) {
    char *duplicate;
    if (str == NULL) {
        return NULL;
    }

    duplicate = malloc(strlen(str) + 1);

    if (duplicate == NULL) {
        return NULL;
    }

    strcpy(duplicate, str);
    return duplicate;
}



void export_error(ErrInfo *err_info) {
    fprintf(stderr,"Error in file %s at line %d: %s\n", err_info->file_name, err_info->line_number, err_info->error_message);

}

ErrInfo *create_error_info(int line_number, char *error_message, char *file_name) {
    ErrInfo *err_info = malloc(sizeof(ErrInfo));
    err_info->line_number = line_number;
    err_info->error_message = duplicate_string(error_message);
    err_info->file_name = duplicate_string(file_name);
    return err_info;
}


Bool is_integer(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if(i==0 && ((str[i] == '-') || (str[i] == '+'))){
            continue;
        }
        if (str[i] < '0' || str[i] > '9') {
            return FALSE;
        }
    }
    return TRUE;
}

Bool is_comment(char *instruction) {
    return instruction[0] == COMMENT_PREFIX;
}

Bool is_empty_row(char *row) {
    int i;
    for (i = 0; i < strlen(row); i++) {
        if (row[i] != ' ' && row[i] != '\t' && row[i] != '\n' && row[i] != '\r') {
            return FALSE;
        }
    }
    return TRUE;
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

/*TODO: CHeck error */
Register get_register(char *operand){
    switch (operand[2]){
        case '0':
            return R0;
        case '1':
            return R1;
        case '2':
            return R2;
        case '3':
            return R3;
        case '4':
            return R4;
        case '5':
            return R5;
        case '6':
            return R6;
        case '7':
            return R7;
        default:
            return INVALID_REGISTER;
    }
}

Bool is_register(char *operand) {
    if (operand[0] == REGISTER_PREFIX ) {
        return TRUE;
    }
    return FALSE;
}

Bool is_label(char *instruction) {
    return instruction[strlen(instruction) - 1] == LABEL_TERMINATOR;
}

int parse_int(char *str) {
    int num;
    sscanf(str, "%d", &num);
    return num;
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