#include "util_types.h"
#include "symbol_table.h"

static Bool rewrite_dynamic_words(AddressingType operand_type, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table, ParsedLine *line, int parameter_index);

static Bool process_line(ParsedLine *line, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table,SymbolTable *externals_table);

Bool rewrite_dynamic_words(AddressingType operand_type, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table, ParsedLine *line, int parameter_index) {
    Bool is_external = FALSE;
    Symbol *label_symbol = NULL;
    Symbol *relocation_symbol = NULL;
    Symbol *external_symbol = NULL;
    switch (operand_type) {
        case (NO_VALUE):
        case (IMMEDIATE):
        case (REGISTER):
            break;
        case (DIRECT):
            label_symbol = get_symbol(labels_table, line->parameters[parameter_index]);
            if ((relocation_symbol = get_symbol(relocations_table, line->parameters[parameter_index]))) {
                is_external = relocation_symbol->type == EXTERN;
            }
            if (label_symbol == NULL && is_external == FALSE) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[parameter_index], "' is not defined"), line->file_name, TRUE);
                return FALSE;
            }
            if (is_external) {
                external_symbol = create_symbol(line->parameters[parameter_index], *ic, EXTERN);
                add_symbol(externals_table, external_symbol);
                parse_symbol_to_word(0, &code_image[*ic], EXTERNAL_ADDRESSING);
            } else {
                parse_symbol_to_word(label_symbol->address, &code_image[*ic], RELOCATABLE_ADDRESSING);
            }
            break;
    }
    return TRUE;
}

Bool process_line(ParsedLine *line, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table,SymbolTable *externals_table) {
    Bool is_success = TRUE;
    Word *current_word = NULL;
    AddressingType src_operand_type = NO_VALUE;
    AddressingType dest_operand_type = NO_VALUE;
    if (line->operand == NULL) {
        return TRUE;
    }
    switch (get_instruction_type(line->operand)) {
        case (ENTRY):
            if(get_symbol(labels_table, line->parameters[0]) == NULL){
                throw_program_error(line->line_number, join_strings(3, "entry '", line->parameters[0], "' is not defined in this file scope"), line->file_name, TRUE);
                return FALSE;
            }
            break;
        case (LABEL):
            /* TODO: validate operand lines are not destroyed */
            throw_program_error(line->line_number, "nested labels are not allowed", line->file_name, FALSE);
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
            CHECK_AND_UPDATE_SUCCESS(is_success, rewrite_dynamic_words(src_operand_type, ic, code_image, labels_table, relocations_table, externals_table, line, 0));
            (*ic) += (src_operand_type != NO_VALUE);
            CHECK_AND_UPDATE_SUCCESS(is_success, rewrite_dynamic_words(dest_operand_type, ic, code_image, labels_table, relocations_table, externals_table, line, src_operand_type == NO_VALUE ? 0 : 1));
            (*ic) += (dest_operand_type != NO_VALUE);
        default:
            break;
    }
    return is_success;
}

Bool second_step_process(Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table, FileOperands *file_operands, int *ic) {
    Bool is_success = TRUE;
    int i = 0;
    ParsedLine *line = NULL;
    for (i = 0; i < file_operands->size; i++) {
        line = &file_operands->lines[i];
        CHECK_AND_UPDATE_SUCCESS(is_success, process_line(line, ic, code_image, labels_table, relocations_table,externals_table));
    }
    return is_success;
}
