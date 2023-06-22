
#include "utils.h"
#include <string.h>
#include <malloc.h>
#include "consts.h"

void remove_end_of_line(char *string){
    int i;
    for(i=0;i<strlen(string);i++){
        if(string[i] == '\n'){
            string[i] = '\0';
            return;
        }
    }
}

void parse_operand_row(char *line, OperandRow *parsed_row) {
    int i;
    const char *delimiters = " ,\n\t";
    strcpy(parsed_row->original_line, line);

    if (line[0] == '\n' || line[0] == '\0') {
        // Empty line, no further processing needed
        parsed_row->operand = NULL;
        parsed_row->parameters = NULL;
        parsed_row->parameters_count = 0;
        return;
    }

    char *operand = strtok(line, delimiters);
    parsed_row->operand = malloc(strlen(operand) + 1);
    strcpy(parsed_row->operand, operand);
    char *parameter = strtok(NULL, delimiters);
    int parameters_count = 0;
    char **parameters = malloc(sizeof(char *));
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
    return file_operands;
}

//TODO: explain the exercise allowed max rows, and maybe add limit
FileOperands *parse_file_to_operand_rows(FILE *file){
    int i;
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
        //resize rows if needed
        rows_count++;
    }
    file_operands->rows = rows;
    file_operands->size = rows_count;
    return file_operands;
}

void remove_row_from_file_operands(FileOperands *file_operands, int row_index){
    int i;
    for(i=row_index;i<file_operands->size - 1;i++){
        file_operands->rows[i] = file_operands->rows[i + 1];
    }
    file_operands->size--;
}