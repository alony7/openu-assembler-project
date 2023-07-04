#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "consts.h"
#include "macros_table.h"
#include "io_parsers.h"


static Bool fill_macro_table(FileOperands *file_operands, MacroTable *table);

static Bool rewrite_macros(FileOperands *file_operands, MacroTable *table, FILE *output_file);

Bool fill_macro_table(FileOperands *file_operands, MacroTable *table) {
    int line_number = 0;
    char *raw_line = NULL;
    ParsedLine parsed_line = {0};
    MacroItem *item = NULL, *tmp_item = NULL;
    Bool is_macro = FALSE;
    for (line_number = 0; line_number < file_operands->size; line_number++) {
        parsed_line = file_operands->lines[line_number];
        raw_line = parsed_line.original_line;
        if (is_comment(raw_line) && !is_macro) {
            continue;
        }
        if (is_string_equals(parsed_line.main_operand, END_MACRO_DIRECTIVE)) {
            if (!is_macro) {
                throw_program_error(line_number, "invalid end of macro",parsed_line.file_name,FALSE);
                return FALSE;
            }
            if (parsed_line.parameters_count != 0) {
                throw_program_error(line_number, join_strings(2,END_MACRO_DIRECTIVE," takes no parameters"),parsed_line.file_name,TRUE);
                return FALSE;
            }
            is_macro = FALSE;
            add_macro_item(table, item);
            continue;
        } else if (is_string_equals(parsed_line.main_operand, START_MACRO_DIRECTIVE)) {
            /*TODO: validate if macro name is caught by command*/
            if (parsed_line.parameters_count != 1) {
                throw_program_error(line_number, join_strings(2,START_MACRO_DIRECTIVE," takes exactly 1 parameter"),parsed_line.file_name,TRUE);
                return FALSE;
            }
            if(get_instruction_type(parsed_line.parameters[0]) != UNKNOWN){
                throw_program_error(line_number, join_strings(2,"macro name cannot be an instruction name: ",parsed_line.parameters[0]),parsed_line.file_name,TRUE);
                return FALSE;
            }
            if (is_macro) {
                throw_program_error(line_number, "nested macros are not allowed",parsed_line.file_name,FALSE);
                return FALSE;
            }
            if ((tmp_item = get_macro_item(table, parsed_line.parameters[0])) != NULL) {
                throw_program_error(line_number, join_strings(3,"macro '",parsed_line.parameters[0],"' already exists"),parsed_line.file_name,TRUE);
                return FALSE;
            }
            item = create_macro_item(parsed_line.parameters[0], NULL, line_number, 0);
            is_macro = TRUE;

        } else {
            if (is_macro) {
                append_macro_item_value(item, raw_line);
            }
        }

    }
    return TRUE;
}

Bool rewrite_macros(FileOperands *file_operands, MacroTable *table, FILE *output_file) {
    int line_number = 0;
    Bool is_macro_line = FALSE,should_free_line_output = FALSE;
    char *line_output = NULL,*raw_line = NULL;
    ParsedLine parsed_line;
    MacroItem *current_item = NULL;
    for (line_number = 0; line_number < file_operands->size; line_number++) {
        parsed_line = file_operands->lines[line_number];
        raw_line = parsed_line.original_line;
        /* comment line */
        if (is_comment(raw_line) || is_empty_line(raw_line)) continue;
        else if (is_string_equals(parsed_line.main_operand, START_MACRO_DIRECTIVE)) is_macro_line = TRUE;
        else if (is_string_equals(parsed_line.main_operand, END_MACRO_DIRECTIVE)) is_macro_line = FALSE;
        else {
            if (is_macro_line) {
                should_free_line_output = FALSE;
                continue;
            }
            else if ((current_item = get_macro_item(table, parsed_line.main_operand)) != NULL) {
                if (line_number < current_item->line_number) {
                    throw_program_error(parsed_line.line_number, join_strings(3,"macro '",current_item->name,"' is used before definition"),parsed_line.file_name,TRUE);
                    return FALSE;
                }
                line_output = join_string_array(current_item->value, current_item->value_size);
                fputs(line_output, output_file);
                free(line_output);
            } else {
                line_output = raw_line;
                fputs(line_output, output_file);
            }


        }
    }
    return TRUE;
}

Bool expand_file_macros(char *input_filename, char *output_filename) {
    FILE *input_file, *output_file;
    Bool result = TRUE;
    FileOperands *parsed_input_file;
    MacroTable *macro_table = NULL;
    input_file = create_file_stream(input_filename, READ_MODE);
    output_file = create_file_stream(output_filename, WRITE_MODE);
    if (!(output_file && input_file)) {
        return FALSE;
    }
    macro_table = create_macro_table();
    parsed_input_file = parse_lines_from_file(input_file, input_filename);
    if ((result = fill_macro_table(parsed_input_file, macro_table))) {
        fseek(input_file, 0, SEEK_SET);
        result = rewrite_macros(parsed_input_file, macro_table, output_file);
    }
    fclose(input_file);
    fclose(output_file);
    free_macro_table(macro_table);
    free_file_operands(parsed_input_file);
    return result;
}



