#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "consts.h"
#include "macros_table.h"

static Bool fill_macro_table(FILE *file, MacroTable *table) {
    char line[MAX_LINE_LENGTH] = {0};
    MacroItem *item;
    Bool is_macro = FALSE;
    char *macro_name;
    int row_number = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        row_number++;
        /* comment line */
        if (line[0] == ';') {
            continue;
        }
        if (strstr(line, START_MACRO_DIRECTIVE) != NULL) {
            is_macro = TRUE;
            macro_name = strtok(line, " ");
            macro_name = strtok(NULL, " ");
            if (macro_name == NULL) {
                //TODO: print error message for invalid macro name
                return FALSE;
            }
            item = create_macro_item(macro_name, NULL, row_number);

        } else {
            //check if end of macro
            if (strstr(line, END_MACRO_DIRECTIVE) != NULL) {
                is_macro = FALSE;
                add_macro_item(table, item);
                continue;
            }
            else if (is_macro) {
                append_macro_item_value(item, line);
            }
        }
    }
    return TRUE;
}

//function to expand the macros in every occurences, based on the macro table
static Bool rewrite_macros(FILE *input_file, FILE *output_file, MacroTable *table){
    char line[MAX_LINE_LENGTH] = {0};
    char *macro_name;
    char *macro_value;

    int row_number = 0;
    Bool is_macro = FALSE;
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        row_number++;
        /* comment line */
        if (line[0] == ';') {
            continue;
        }
        if (strstr(line, START_MACRO_DIRECTIVE) != NULL) {
            is_macro = TRUE;
            continue;
}
        else{
            if (strstr(line, END_MACRO_DIRECTIVE) != NULL) {
                is_macro = FALSE;
                continue;
            }
            //append line to output file
            else if (!is_macro) {
                //if current line value is a macro
                //TODO: check if macro is valid
                //TODO: check if macro is defined
                //TODO: check if macro line is not before macro definition
                if (get_macro_item(table, line) != NULL) {
                    macro_name = strtok(line, " ");
                    //TODO: write all lines in macro 2d array
                    macro_value = get_macro_item(table, macro_name)->value;
                    fputs(macro_value, output_file);
                }
                else{
                    fputs(line, output_file);}
            }
        }
    }
}

static Bool expand_file_macros(char *filename) {

}

Bool expand_macros(char *filenames[], int num_of_files) {
    int i;
    char filename_with_extension[256];
    FILE *current_file;
    for (i = 0; i < num_of_files; i++) {
        /* TODO: separate to new method */
        /* TODO: check buffer*/
        strcpy(filename_with_extension, filenames[i]);
        strcat(filename_with_extension, AS_FILE_EXTENSION);
        /*TODO: Output error message */
        if ((current_file = fopen(filename_with_extension, "r")) == NULL) {
            //print error message for failed file opening
            printf("Error: Failed to open file %s\n", filename_with_extension);
            continue;
        }
        /*TODO: expand macro */
        printf("File: %s\n", filename_with_extension);
        expand_file_macros(filename_with_extension);
    }
    return TRUE;
}
