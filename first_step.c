//
// Created by alons on 23/06/2023.
//

#include <string.h>
#include "first_step.h"
#include "instruction_handling.h"


Bool handle_row(SymbolTable table, OperandRow *row, Word *data_image, Word *code_image, int *ic, int *dc) {
    int i;
    Symbol *symbol;
    InstructionType instruction_type, label_instruction_type;
    if((instruction_type = get_instruction_type(row->operand))==LABEL){
        /*TODO:  validate label doesnt exist */
        label_instruction_type = get_instruction_type(row->parameters[0]);
        switch (label_instruction_type) {
            case (DATA):
                symbol = create_symbol(row->operand, *dc, DATA);
                add_symbol(&table, symbol);
                break;
            case (STRING):
                symbol = create_symbol(row->operand, *dc, STRING);
                add_symbol(&table, symbol);
                break;
            case (COMMAND):
                symbol = create_symbol(row->operand, *ic, COMMAND);
                add_symbol(&table, symbol);
                break;
            case (COMMENT):
                return TRUE;
            default:
                /* TODO: throw error */
        }
       symbol = create_symbol(row->operand, label_instruction_type == COMMAND?*ic:*dc, COMMAND);
       add_symbol(&table, symbol);
       row->operand = row->parameters[0];
       for(i=0;i<row->parameters_count-1;i++){
           row->parameters[i] = row->parameters[i+1];
       }
    };
    if(instruction_type == COMMENT) {
        return TRUE;
    }else if(instruction_type == DATA){
        address_data_instruction(row,data_image,dc);
    }else if(instruction_type == STRING) {
        address_string_instruction(row,data_image,dc);
    }else if(instruction_type == LABEL) {
        /* TODO: throw error for double label */
    }else if(instruction_type == EXTERN) {
        /* TODO: add to extern table */
    }else if(instruction_type == ENTRY) {
        /* TODO: add to entry table */
    }else if(instruction_type == COMMAND){
        address_code_instruction(row,code_image,ic);
    }else{
        return FALSE;
    }
    return TRUE;
}

Bool first_step_process(SymbolTable table, FileOperands *file_operands, char *file_name) {
    int ic = 0;
    int dc = 0;
    Word data_image[MEMORY_SIZE];
    Word code_image[MEMORY_SIZE];
    Bool is_successful = TRUE;
    int i;
    OperandRow *current_row;
    FILE *file = create_file_stream(file_name, "r");
    if (file == NULL) {
        return FALSE;
    }
    file_operands = parse_file_to_operand_rows(file);
    for (i = 0; i < file_operands->size; i++) {
        current_row = &file_operands->rows[i];
        is_successful = handle_row(table, current_row,data_image,code_image, &ic, &dc) && is_successful;
    }

    fclose(file);
    return is_successful;
}

