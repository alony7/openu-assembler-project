
#include "utils.h"
#include <string.h>
#include "io_parsers.h"
#include "consts.h"
#include "memory_wrappers.h"


FILE *create_file_stream(char *file_name, char *mode){
    /* Open the file */
    FILE *file = fopen(file_name, mode);
    /* Check if the file was opened successfully */
    if (file == NULL) {
        /* Throw a system error */
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
    CLEAN_STRING(parsed_line->original_line);

    if (line[0] == EOL || line[0] == NULL_CHAR) {
        /* Empty line, no further processing needed*/
        parsed_line->main_operand = NULL;
        parsed_line->parameters = NULL;
        parsed_line->parameters_count = 0;
        parsed_line->line_number = 0;
        return;
    }
    /* extract the first token */
    operand = strtok(line, delimiters);
    parsed_line->main_operand = (char *) safe_malloc(strlen(operand) + 1);
    strcpy(parsed_line->main_operand, operand);
    /* clean the string from crlf and add null terminator */
    CLEAN_STRING(parsed_line->main_operand);
    parameter = strtok(NULL, delimiters);
    CLEAN_STRING(parameter);
    parameters_count = 0;
    parameters = (char **) safe_malloc(sizeof(char *));
    /* extract the rest of the tokens */
    while (parameter != NULL) {
        parameters_count++;
        /* allocate memory for the new parameter */
        parameters = (char **) safe_realloc(parameters, parameters_count * sizeof(char *));
        /* copy the parameter to the array */
        parameters[parameters_count - 1] = (char *) safe_malloc(strlen(parameter) + 1);
        strcpy(parameters[parameters_count - 1], parameter);
        parameter = strtok(NULL, delimiters);
        /* clean the string from crlf and add null terminator */
        CLEAN_STRING(parameter);
    }
    parsed_line->parameters = parameters;
    parsed_line->parameters_count = parameters_count;
    parsed_line->line_number = 0;
    /* remove the last parameter if it's empty */
    if(parameters_count && (*(parameters[parameters_count-1]) == 0)){
        free(parameters[parameters_count-1]);
        parsed_line->parameters_count--;
    }
}


void free_parsed_line(ParsedLine *line) {
    int i;
    /* free the main operand */
    free(line->main_operand);
    for (i = 0; i < line->parameters_count; i++) {
        /* free each parameter */
        if(*line->parameters[i] != NULL_CHAR){
            free(line->parameters[i]);
        }
    }
    /* free the parameter pointers array */
    free(line->parameters);
}


void build_output_filename(char *base_name, char *suffix, char *output_buffer) {
    /* copy the base name to the output buffer */
    strcpy(output_buffer, base_name);
    /* append the suffix */
    strcat(output_buffer, suffix);
}

void clear_current_line(FILE *stream){
    char c;
    while((c = fgetc(stream)) != EOL && c != EOF);
}
