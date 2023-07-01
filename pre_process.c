#include <stdio.h>
#include <string.h>
#include "consts.h"
#include "macros_table.h"
#include "io_parsers.h"


static Bool fill_macro_table(FileOperands *file_operands, MacroTable *table);

static Bool rewrite_macros(FileOperands *file_operands, MacroTable *table, FILE *output_file);

/*TODO: refactor printf to error handling*/
Bool fill_macro_table(FileOperands *file_operands, MacroTable *table) {
    int line_number = 0;
    char *raw_line;
    ParsedLine parsed_line;
    MacroItem *item;
    MacroItem *tmp_item = NULL;
    Bool is_macro = FALSE;
    for (line_number = 0; line_number < file_operands->size; line_number++) {
        parsed_line = file_operands->lines[line_number];
        raw_line = parsed_line.original_line;
        if (is_comment(raw_line) && !is_macro) {
            continue;
        }
        if (is_string_equals(parsed_line.operand, END_MACRO_DIRECTIVE)) {
            if (!is_macro) {
                printf("Error: invalid end of macro in line %d\n", line_number);
                return FALSE;
            }
            if(parsed_line.parameters_count != 0){
                printf("Error: invalid number of parameters for '%s' in line %d\n", END_MACRO_DIRECTIVE, line_number);
                return FALSE;
            }
            is_macro = FALSE;
            add_macro_item(table, item);
            continue;
        } else if (is_string_equals(parsed_line.operand, START_MACRO_DIRECTIVE)) {
            /*TODO: validate if macro name is caught by command*/
            if (parsed_line.parameters_count != 1) {
                printf("Error: invalid number of parameters for '%s' in line %d\n", START_MACRO_DIRECTIVE, line_number);
                return FALSE;
            }
            if (is_macro) {
                printf("Error: nested macro in line %d\n", line_number);
                return FALSE;
            }
            if ((tmp_item = get_macro_item(table, parsed_line.parameters[0])) != NULL) {
                printf("Error: macro '%s' already exists in line %d\n", tmp_item->name, tmp_item->line_number);
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

/*TODO: Check if i need to delete empty lines and comments*/
Bool rewrite_macros(FileOperands *file_operands, MacroTable *table, FILE *output_file) {
    int line_number = 0;
    Bool is_macro = FALSE;
    char *line_output = {0};
    char *raw_line = {0};
    ParsedLine parsed_line;
    MacroItem *current_item = NULL;
    for (line_number = 0; line_number < file_operands->size; line_number++) {
        parsed_line = file_operands->lines[line_number];
        raw_line = line_output = parsed_line.original_line;
        /* comment line */
        if (is_comment(raw_line) || is_empty_line(raw_line)) {
            continue;
        } else if (is_string_equals(parsed_line.operand, START_MACRO_DIRECTIVE)) {
            is_macro = TRUE;
        } else if (is_string_equals(parsed_line.operand, END_MACRO_DIRECTIVE)) {
            is_macro = FALSE;
        }
        else {
            if (!is_macro) {
                if (get_macro_item(table, parsed_line.operand) != NULL) {
                    current_item = get_macro_item(table, parsed_line.operand);
                    if (line_number < current_item->line_number) {
                        printf("Error: macro '%s' is used before definition in line %d\n", current_item->name,
                               line_number);
                        return FALSE;
                    }
                    line_output = string_array_to_string(current_item->value, current_item->value_size);

                } else {
                    line_output = raw_line;
                }
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



