//
// Created by alons on 22/06/2023.
//

#include "utils.h"
#include <string.h>
#include <malloc.h>
#include "input_utils.h"

void remove_end_of_line(char *string){
    int i;
    for(i=0;i<strlen(string);i++){
        if(string[i] == '\n'){
            string[i] = '\0';
            return;
        }
    }
}

void *parse_operand_row(char *line, OperandRow *parsed_row){
    int i;
    const char *delimiters = " ,\n\t";
    char *operand = strtok(line, delimiters);
    parsed_row->operand = malloc(strlen(operand) + 1);
    strcpy(parsed_row->operand, operand);
    char *parameter = strtok(NULL, delimiters);
    int parameters_count = 0;
    char **parameters = malloc(sizeof(char *));
    while(parameter != NULL){
        parameters_count++;
        parameters = realloc(parameters, parameters_count * sizeof(char *));
        parameters[parameters_count - 1] = parameter;
        parameter = strtok(NULL, delimiters);
    }
    parsed_row->parameters = parameters;
    parsed_row->parameters_count = parameters_count;
    return parsed_row;
}
