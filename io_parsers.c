
#include "utils.h"
#include <string.h>
#include <malloc.h>
#include "consts.h"


FILE *create_file_stream(char *file_name, char *mode) {
    FILE *file = fopen(file_name, mode);
    if (file == NULL) {
        return NULL;
    }
    return file;
}

void parse_operand_row(char *line, OperandRow *parsed_row) {
    char *operand;
    char *parameter;
    char **parameters;
    int parameters_count;
    const char *delimiters = " ,\n\t";
    strcpy(parsed_row->original_line, line);

    if (line[0] == '\n' || line[0] == '\0') {
        /* Empty line, no further processing needed*/
        parsed_row->operand = NULL;
        parsed_row->parameters = NULL;
        parsed_row->parameters_count = 0;
        return;
    }
    operand = strtok(line, delimiters);
    parsed_row->operand = malloc(strlen(operand) + 1);
    strcpy(parsed_row->operand, operand);
    parameter = strtok(NULL, delimiters);
    parameters_count = 0;
    parameters = malloc(sizeof(char *));
    while (parameter != NULL) {
        parameters_count++;
        parameters = realloc(parameters, parameters_count * sizeof(char *));
        parameters[parameters_count - 1] = malloc(strlen(parameter) + 1);
        strcpy(parameters[parameters_count - 1], parameter);
        parameter = strtok(NULL, delimiters);
    }
    parsed_row->parameters = parameters;
    parsed_row->parameters_count = parameters_count;
}

static FileOperands *create_file_operands(){
    FileOperands *file_operands = malloc(sizeof(FileOperands));
    file_operands->size = 0;
    file_operands->rows = NULL;
    file_operands->file_name = NULL;
    return file_operands;
}

/*TODO: explain the exercise allowed max rows, and maybe add limit
 * TODO: make this take just the filename*/
FileOperands *parse_file_to_operand_rows(FILE *file,char *file_name){
    FileOperands *file_operands = create_file_operands();
    OperandRow *rows = malloc(BASE_ROW_BATCH_SIZE * sizeof(OperandRow));
    OperandRow *current_row;
    char line[MAX_LINE_LENGTH] = {0};
    char tmp_line[MAX_LINE_LENGTH] = {0};
    int rows_count = 0;
    int rows_capacity = BASE_ROW_BATCH_SIZE;
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        if(rows_count == rows_capacity){
            rows_capacity += BASE_ROW_BATCH_SIZE;
            rows = realloc(rows, rows_capacity * sizeof(OperandRow));
        }
        current_row = rows + rows_count;
        strcpy(tmp_line, line);

        parse_operand_row(tmp_line, current_row);
        /*resize rows if needed*/
        current_row->line_number = rows_count;
        strcpy(current_row->file_name, file_name);
        rows_count++;
    }
    file_operands->rows = rows;
    file_operands->size = rows_count;
    return file_operands;
}


void free_operand_row(OperandRow *row){
    int i;
    free(row->operand);
    for(i=0;i<row->parameters_count;i++){
        free(row->parameters[i]);
    }
    free(row->parameters);
}

void free_file_operands(FileOperands *file_operands){
    int i;
    for(i=0;i<file_operands->size;i++){
        free_operand_row(file_operands->rows + i);
    }
    free(file_operands->rows);
    free(file_operands);
}