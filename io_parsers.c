
#include "utils.h"
#include <string.h>
#include "io_parsers.h"
#include "consts.h"
#include "memory_wrappers.h"


FILE *create_file_stream(char *file_name, char *mode) {
    FILE *file = fopen(file_name, mode);
    if (file == NULL) {
        throw_system_error(join_strings(3, "failed to open file: '", file_name, "'"), TRUE);
        return NULL;
    }
    return file;
}

void parse_line(char *line, ParsedLine *parsed_line) {
    char *operand;
    char *parameter;
    char **parameters;
    int parameters_count;
    const char *delimiters = " ,\n\t";
    strcpy(parsed_line->original_line, line);

    if (line[0] == '\n' || line[0] == '\0') {
        /* Empty line, no further processing needed*/
        parsed_line->main_operand = NULL;
        parsed_line->parameters = NULL;
        parsed_line->parameters_count = 0;
        return;
    }
    operand = strtok(line, delimiters);
    parsed_line->main_operand = (char *) safe_malloc(strlen(operand) + 1);
    strcpy(parsed_line->main_operand, operand);
    CLEAN_STRING(parsed_line->main_operand);
    parameter = strtok(NULL, delimiters);
    CLEAN_STRING(parameter);
    parameters_count = 0;
    parameters = (char **) safe_malloc(sizeof(char *));
    while (parameter != NULL) {
        parameters_count++;
        parameters = (char **) safe_realloc(parameters, parameters_count * sizeof(char *));
        parameters[parameters_count - 1] = (char *) safe_malloc(strlen(parameter) + 1);
        strcpy(parameters[parameters_count - 1], parameter);
        parameter = strtok(NULL, delimiters);
        CLEAN_STRING(parameter);
    }
    parsed_line->parameters = parameters;
    parsed_line->parameters_count = parameters_count;
    if(parameters_count && *(parameters[parameters_count-1]) == NULL){
        free(parameters[parameters_count-1]);
        parsed_line->parameters_count--;
    }
}


void free_parsed_line(ParsedLine *line) {
    int i;
    free(line->main_operand);
    for (i = 0; i < line->parameters_count; i++) {
        free(line->parameters[i]);
    }
    free(line->parameters);
}


void build_output_filename(char *base_name, char *suffix, char *output_buffer) {
    strcpy(output_buffer, base_name);
    strcat(output_buffer, suffix);
}

