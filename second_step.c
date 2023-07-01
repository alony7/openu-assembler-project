#include "util_types.h"
#include "symbol_table.h"


static Bool rewrite_operand_word(AddressingType operand_type, int *ic, Word *code_image, SymbolTable *labels_table,
                                 SymbolTable *relocations_table, OperandRow *row, int parameter_index) {
    Bool is_external = FALSE;
    Symbol *label_symbol = NULL;
    Symbol *relocation_symbol = NULL;
    switch (operand_type) {
        case (NO_VALUE):
        case (IMMEDIATE):
        case (REGISTER):
            break;
        case (DIRECT):
            label_symbol = get_symbol(labels_table, row->parameters[parameter_index]);
            if ((relocation_symbol = get_symbol(relocations_table, row->parameters[parameter_index]))) {
                is_external = relocation_symbol->type == EXTERN;
            }
            if (label_symbol == NULL && is_external == FALSE) {
                throw_program_error(row->line_number,join_strings(3, "label '", row->parameters[parameter_index], "' is not defined"),row->file_name, TRUE);
                return FALSE;
            }
            if (is_external) {
                parse_symbol_to_word(0, &code_image[*ic], EXTERNAL_ADDRESSING);
            } else {
                parse_symbol_to_word(label_symbol->address, &code_image[*ic], RELOCATABLE_ADDRESSING);
            }
            break;
    }
    return TRUE;
}

static Bool process_line(OperandRow *row, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table) {
    Bool is_success = TRUE;
    Word *current_word = NULL;
    AddressingType src_operand_type = NO_VALUE;
    AddressingType dest_operand_type = NO_VALUE;
    if (row->operand == NULL) {
        return TRUE;
    }
    switch (get_instruction_type(row->operand)) {
        case (LABEL):
            /* TODO: validate operand rows are not destroyed */
            throw_program_error(row->line_number, "nested labels are not allowed", row->file_name, FALSE);
            return FALSE;
        case (COMMAND):

            current_word = &code_image[*ic];
            get_word_addressing_types(current_word, &src_operand_type, &dest_operand_type);
            (*ic)++;
            if (src_operand_type == NO_VALUE && dest_operand_type == NO_VALUE) {
                return TRUE;
            }
            if (src_operand_type == REGISTER && dest_operand_type == REGISTER) {
                (*ic)++;
                return TRUE;
            }
            CHECK_AND_UPDATE_SUCCESS(is_success, rewrite_operand_word(src_operand_type, ic, code_image, labels_table,relocations_table, row, 0));
            (*ic) += (src_operand_type != NO_VALUE);
            CHECK_AND_UPDATE_SUCCESS(is_success, rewrite_operand_word(dest_operand_type, ic, code_image, labels_table,relocations_table, row,src_operand_type == NO_VALUE ? 0 : 1));
            (*ic) += (dest_operand_type != NO_VALUE);
        default:
            break;
    }
    return is_success;
}

Bool second_step_process(Word code_image[MEMORY_SIZE], SymbolTable *labels_table,
                         SymbolTable *relocations_table, FileOperands *file_operands, char *file_name) {
    Bool is_success = TRUE;
    int i = 0;
    int ic = MEMORY_OFFSET;
    OperandRow *row = NULL;
    for (i = 0; i < file_operands->size; i++) {
        row = &file_operands->rows[i];
        CHECK_AND_UPDATE_SUCCESS(is_success, process_line(row, &ic, code_image, labels_table, relocations_table));
    }
    return is_success;
}
