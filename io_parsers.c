
#include "utils.h"
#include <string.h>
#include "io_parsers.h"
#include "consts.h"
#include "memory_wrappers.h"

static void parse_line(char *line, ParsedLine *parsed_line);

static void free_parsed_line(ParsedLine *line);

static FileOperands *create_file_operands();

FILE *create_file_stream(char *file_name, char *mode) {
    FILE *file = fopen(file_name, mode);
    if (file == NULL) {
        throw_system_error(join_strings(3, "failed to open file: '", file_name, "'"), TRUE);
        return NULL;
    }
    return file;
}

/*TODO: add string terminations */
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
}

FileOperands *create_file_operands() {
    FileOperands *file_operands = (FileOperands *) safe_malloc(sizeof(FileOperands));
    file_operands->size = 0;
    file_operands->lines = NULL;
    return file_operands;
}

/*TODO: explain the exercise allowed max lines, and maybe add limit
 * TODO: make this take just the filename*/
FileOperands *parse_lines_from_file(FILE *file, char *file_name) {
    FileOperands *file_operands = create_file_operands();
    ParsedLine *lines = (ParsedLine *) safe_malloc(BASE_LINE_BATCH_SIZE * sizeof(ParsedLine));
    ParsedLine *current_line;
    char line[MAX_LINE_LENGTH] = {0};
    char tmp_line[MAX_LINE_LENGTH] = {0};
    int lines_count = 0;
    int lines_capacity = BASE_LINE_BATCH_SIZE;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (lines_count == lines_capacity) {
            lines_capacity += BASE_LINE_BATCH_SIZE;
            lines = (ParsedLine *) safe_realloc(lines, lines_capacity * sizeof(ParsedLine));
        }
        current_line = lines + lines_count;
        strcpy(tmp_line, line);

        parse_line(tmp_line, current_line);
        /*resize lines if needed*/
        current_line->line_number = lines_count;
        strcpy(current_line->file_name, file_name);
        lines_count++;
    }
    file_operands->lines = lines;
    file_operands->size = lines_count;
    return file_operands;
}


void free_parsed_line(ParsedLine *line) {
    int i;
    free(line->main_operand);
    for (i = 0; i < line->parameters_count; i++) {
        free(line->parameters[i]);
    }
    free(line->parameters);
}

void free_file_operands(FileOperands *file_operands) {
    int i;
    for (i = 0; i < file_operands->size; i++) {
        free_parsed_line(file_operands->lines + i);
    }
    free(file_operands->lines);
    free(file_operands);
}

void build_output_filename(char *base_name, char *suffix, char *output_buffer) {
    strcpy(output_buffer, base_name);
    strcat(output_buffer, suffix);
}

