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
        if (str[i] < '0' || str[i] > '9') {
            return FALSE;
        }
    }
    return TRUE;
}

Bool is_comment(char *instruction) {
    return instruction[0] == COMMENT_PREFIX;
}