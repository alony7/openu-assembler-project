#include <stdio.h>
#include <string.h>
#include "consts.h"
#include "macros_table.h"
#include "io_parsers.h"


static Bool is_string_equal(const char *line, const char *directive) {
    if (line == NULL || directive == NULL) {
        return FALSE;
    }

    return !strcmp(line, directive);
}

static Bool fill_macro_table(FileOperands *file_operands, MacroTable *table) {
    int row_number = 0;
    char *raw_line;
    OperandRow parsed_row;
    MacroItem *item;
    MacroItem *tmp_item = NULL;
    Bool is_macro = FALSE;
    for (row_number = 0; row_number < file_operands->size; row_number++) {
        parsed_row = file_operands->rows[row_number];
        raw_line = parsed_row.original_line;
        if (is_comment(raw_line) && !is_macro) {
            continue;
        }
        if (is_string_equal(parsed_row.operand, END_MACRO_DIRECTIVE)) {
            if (!is_macro) {
                printf("Error: invalid end of macro in line %d\n", row_number);
                return FALSE;
            }
            if(parsed_row.parameters_count != 0){
                printf("Error: invalid number of parameters for '%s' in line %d\n", END_MACRO_DIRECTIVE, row_number);
                return FALSE;
            }
            is_macro = FALSE;
            add_macro_item(table, item);
            continue;
        } else if (is_string_equal(parsed_row.operand, START_MACRO_DIRECTIVE)) {
            /*TODO: validate if macro name is caught by command*/
            if (parsed_row.parameters_count != 1) {
                printf("Error: invalid number of parameters for '%s' in line %d\n", START_MACRO_DIRECTIVE, row_number);
                return FALSE;
            }
            if (is_macro) {
                printf("Error: nested macro in line %d\n", row_number);
                return FALSE;
            }
            if ((tmp_item = get_macro_item(table, parsed_row.parameters[0])) != NULL) {
                printf("Error: macro '%s' already exists in line %d\n", tmp_item->name, tmp_item->row_number);
                return FALSE;
            }
            item = create_macro_item(parsed_row.parameters[0], NULL, row_number, 0);
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
static Bool rewrite_macros(FileOperands *file_operands, MacroTable *table, FILE *output_file) {
    int row_number = 0;
    Bool is_macro = FALSE;
    char *line_output = {0};
    char *raw_line = {0};
    OperandRow parsed_row;
    MacroItem *current_item = NULL;
    for (row_number = 0; row_number < file_operands->size; row_number++) {
        parsed_row = file_operands->rows[row_number];
        raw_line = line_output = parsed_row.original_line;
        /* comment line */
        if (is_comment(raw_line)) {
            continue;
        } else if (is_string_equal(parsed_row.operand, START_MACRO_DIRECTIVE)) {
            is_macro = TRUE;
        } else if (is_string_equal(parsed_row.operand, END_MACRO_DIRECTIVE)) {
            is_macro = FALSE;
        }
        else {
            if (!is_macro) {
                if (parsed_row.operand == NULL) {
                    line_output = raw_line;
                } else if (get_macro_item(table, parsed_row.operand) != NULL) {
                    current_item = get_macro_item(table, parsed_row.operand);
                    if (row_number < current_item->row_number) {
                        printf("Error: macro '%s' is used before definition in line %d\n", current_item->name,
                               row_number);
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

static Bool expand_file_macros(char *input_filename, char *output_filename) {
    FILE *input_file, *output_file;
    Bool result = TRUE;
    input_file = create_file_stream(input_filename, READ_MODE);
    output_file = create_file_stream(output_filename, WRITE_MODE);
    if (!output_file) {
        printf("Error: Failed to create file %s\n", output_filename);
        return FALSE;
    }
    MacroTable macro_table = create_macro_table();
    FileOperands *parsed_input_file = parse_file_to_operand_rows(input_file);
    if ((result = fill_macro_table(parsed_input_file, &macro_table))) {
        fseek(input_file, 0, SEEK_SET);
        result = rewrite_macros(parsed_input_file, &macro_table, output_file);
    }
    fclose(input_file);
    fclose(output_file);
    free_macro_table(&macro_table);
    free_file_operands(parsed_input_file);
    return result;
}


static void build_output_filename(char *base_name, char *suffix, char *output_buffer) {
    strcpy(output_buffer, base_name);
    strcat(output_buffer, suffix);
}

Bool expand_macros(char *filenames[], int num_of_files) {
    int i;
    char filename_with_as_extension[MAX_FILENAME_LENGTH];
    char filename_with_am_extension[MAX_FILENAME_LENGTH];
    for (i = 0; i < num_of_files; i++) {
        build_output_filename(filenames[i], AM_FILE_EXTENSION, filename_with_am_extension);
        build_output_filename(filenames[i], AS_FILE_EXTENSION, filename_with_as_extension);
        printf("Opening File: %s\n", filename_with_as_extension);

        if (expand_file_macros(filename_with_as_extension, filename_with_am_extension)) {
            printf("File %s expanded successfully\n", filename_with_as_extension);
        } else {
            printf("Error: Failed to expand file %s\n", filename_with_as_extension);
        }
    }
    return TRUE;
}
