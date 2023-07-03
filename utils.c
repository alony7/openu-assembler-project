#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include "instruction_handling.h"
#include "error.h"
#include "memory_wrappers.h"

char base64_encode(int value);

char *string_array_to_string(char **array, int size) {
    int i;
    char *string = (char *) safe_malloc(sizeof(char));
    string[0] = NULL_CHAR;
    for (i = 0; i < size; i++) {
        string = (char *) safe_realloc(string, (strlen(string) + strlen(array[i]) + 1) * sizeof(char));
        strcat(string, array[i]);
    }
    return string;
}

Bool is_string_equals(const char *line, const char *directive) {
    if (line == NULL || directive == NULL) {
        return FALSE;
    }

    return !strcmp(line, directive);
}

char *duplicate_string(const char *str) {
    char *duplicate;
    if (str == NULL) {
        return NULL;
    }

    duplicate = (char *) safe_malloc(strlen(str) + 1);

    if (duplicate == NULL) {
        return NULL;
    }

    strcpy(duplicate, str);
    return duplicate;
}


Bool is_integer(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (i == 0 && ((str[i] == '-') || (str[i] == '+'))) {
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

Bool is_empty_line(char *line) {
    int i;
    for (i = 0; i < strlen(line); i++) {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\r') {
            return FALSE;
        }
    }
    return TRUE;
}

/* TODO: clean this */
InstructionCode get_instruction_code(char *command) {
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
    } else if (strcmp(command, JMP_COMMAND) == 0)
        return JMP;
    else if (strcmp(command, BNE_COMMAND) == 0)
        return BNE;
    else if (strcmp(command, RED_COMMAND) == 0)
        return RED;
    else if (strcmp(command, PRN_COMMAND) == 0)
        return PRN;
    else if (strcmp(command, JSR_COMMAND) == 0)
        return JSR;
    else if (strcmp(command, RTS_COMMAND) == 0)
        return RTS;
    else if (strcmp(command, STOP_COMMAND) == 0)
        return STOP;
    return INVALID_INSTRUCTION;
}

Register get_register(char *operand) {
    if (!is_register(operand) || operand[1] != REGISTER_LETTER) {
        return INVALID_REGISTER;
    }
    if (strlen(operand) != 3) {
        return INVALID_REGISTER;
    }

    switch (operand[2]) {
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

Bool is_register(const char *operand) {
    if (operand[0] == REGISTER_PREFIX) {
        return TRUE;
    }
    return FALSE;
}

Bool is_label(char *instruction) {
    return instruction[strlen(instruction) - 1] == LABEL_TERMINATOR;
}

/* TODO: report error */
int parse_int(char *str) {
    int num;
    sscanf(str, "%d", &num);
    return num;
}

/* TODO: free memory */
char *join_strings(int num_strings, ...) {
    va_list args;
    unsigned int total_length = 0;
    char *result, *current;
    int i;

    va_start(args, num_strings);
    for (i = 0; i < num_strings; i++) {
        current = va_arg(args, char *);
        total_length += strlen(current);
    }
    va_end(args);

    result = (char *) safe_malloc((total_length + 1) * sizeof(char));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    va_start(args, num_strings);
    result[0] = '\0';
    for (i = 0; i < num_strings; i++) {
        current = va_arg(args, char *);
        strcat(result, current);
    }
    va_end(args);

    return result;
}

void trim_string_quotes(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        str[i] = str[i + 1];
    }
    str[strlen(str) - 1] = '\0';
}

char base64_encode(int value) {
    if (value < 26) {
        return 'A' + value;
    } else if (value < 52) {
        return 'a' + (value - 26);
    } else if (value < 62) {
        return '0' + (value - 52);
    } else if (value == 62) {
        return '+';
    } else {
        return '/';
    }
}

void word_to_base64(Word *word, char *base64) {
    int first6Bits = 0;
    int second6Bits = 0;
    int i;

    for (i = 5; i >= 0; i--) {
        first6Bits += (word->bits[i] << (i));
    }

    for (i = 11; i >= 6; i--) {
        second6Bits += (word->bits[i] << (i - 6));
    }

    base64[0] = base64_encode(second6Bits);

    base64[1] = base64_encode(first6Bits);

    base64[2] = NULL_CHAR;
}
