#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include "instruction_handling.h"
#include "error.h"

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

Register get_register(char *operand){
    if(!is_register(operand) || operand[1] != REGISTER_LETTER){
        return INVALID_REGISTER;
    }
    if(strlen(operand) != 3){
        return INVALID_REGISTER;
    }

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

/* TODO: free memory */
char *join_strings(int num_strings, ...) {
    va_list args;
    int total_length = 0;
    char *result, *current;
    int i;

    // Calculate the total length of all strings
    va_start(args, num_strings);
    for (i = 0; i < num_strings; i++) {
        current = va_arg(args, char *);
        total_length += strlen(current);
    }
    va_end(args);

    // Allocate memory for the result string
    result = (char *)malloc((total_length + 1) * sizeof(char));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Concatenate all the strings
    va_start(args, num_strings);
    result[0] = '\0';  // Initialize the result string as an empty string
    for (i = 0; i < num_strings; i++) {
        current = va_arg(args, char *);
        strcat(result, current);
    }
    va_end(args);

    return result;
}