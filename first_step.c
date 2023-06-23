//
// Created by alons on 23/06/2023.
//

#include <string.h>
#include "first_step.h"


static Bool is_label(char *operand){
    return operand[strlen(operand) - 1] == ':';
}

InstructionType get_instruction_type(char *instruction){
    if(strcmp(instruction,".data") == 0){
        return DATA;
    }
    if(strcmp(instruction,".string") == 0){
        return STRING;
    }
    if(strcmp(instruction,".extern") == 0){
        return EXTERN;
    }
    if(strcmp(instruction,".entry") == 0){
        return ENTRY;
    }
    return COMMAND;
}


Bool first_step(SymbolTable table,FileOperands *file_operands, char *file_name){
    int ic = 0;
    int dc= 0;
    Bool label_flag = FALSE;
    int i;
    OperandRow *current_row;
    Symbol *symbol;
    InstructionType instruction_type;
    FILE *file = create_file_stream(file_name, "r");
    if (file == NULL) {
        return FALSE;
    }
    file_operands = parse_file_to_operand_rows(file);
    for(i=0;i<file_operands->size;i++){
        current_row = &file_operands->rows[i];
        if(is_label(current_row->operand)){
            label_flag = TRUE;
            instruction_type = get_instruction_type(current_row->parameters[0]);
            //TODO: check if symbol exists
            if(instruction_type == DATA || instruction_type == STRING){
                symbol = create_symbol(current_row->operand, dc,instruction_type );
                if(!add_symbol(&table, symbol)){
                    return FALSE;
                }
                dc += strlen(current_row->original_line);
            }else if( instruction_type == EXTERN || instruction_type == ENTRY){
                if(instruction_type == EXTERN){
                    symbol = create_symbol(current_row->operand, 0, instruction_type);
                    if(!add_symbol(&table, symbol)){
                        return FALSE;
                    }
                }
                else{
                    symbol = get_symbol(&table, current_row->operand);
                    if(symbol == NULL){
                        return FALSE;
                    }
                    symbol->type = ENTRY;
                }
        }
    }else{

        }
    }

    fclose(file);
    return TRUE;
}

