#include "output_generator.h"

/**
 * Exports an array of words to a file.
 *
 * @param words The array of words to export.
 * @param num_of_words The number of words to export (the array length).
 * @param file The file stream to export the words to.
 */
static void export_words_to_file(Word *words, int num_of_words, FILE *file);

/**
 * Generates the object file containing the code image followed by the data image.
 *
 * @param code_image The code image.
 * @param data_image The data image.
 * @param object_file The file stream to write the object file.
 * @param num_of_command_words The number of words in the code image.
 * @param num_of_data_words The number of words in the data image.
 */
static void generate_object_file(Word *code_image, Word *data_image, FILE *object_file, int num_of_command_words, int num_of_data_words);

/**
 * Generates the entries file containing the entry definitions from the relocation table.
 *
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param symbol_table The symbol table for labels.
 * @param entry_file The file stream to write the entries file.
 */
static void generate_entries_file(const SymbolTable *relocation_table, SymbolTable *symbol_table, FILE *entry_file);

/**
 * Generates the externals file containing the occurrences of external symbols in the code image.
 *
 * @param externals_table The symbol table for externals.
 * @param extern_file The file stream to write the externals file.
 */
static void generate_externals_file(const SymbolTable *externals_table, FILE *extern_file);


Bool generate_output_files(char *object_filename, char *entry_filename, char *extern_filename, SymbolTable *relocation_table, SymbolTable *symbol_table, SymbolTable *externals_table, Word *code_image, Word *data_image, int dc, int ic) {
    FILE *object_file, *entry_file, *extern_file;
    /* calculate the number of words in the code image */
    int num_of_command_words = ic - MEMORY_OFFSET;

    /* if there are external definitions, generate the externals file */
    if (count_symbols_by_type(relocation_table, EXTERN)) {
        /* create the file stream */
        extern_file = create_file_stream(extern_filename, WRITE_MODE);
        /* if the file could not be created, return FALSE */
        if (extern_file == NULL) return FALSE;
        /* generate the externals file */
        generate_externals_file(externals_table, extern_file);
        /* close the file stream */
        fclose(extern_file);

    }
    /* if there are entry definitions, generate the entries file */
    if (count_symbols_by_type(relocation_table, ENTRY)) {
        /* create the file stream */
        entry_file = create_file_stream(entry_filename, WRITE_MODE);
        /* if the file could not be created, return FALSE */
        if (entry_file == NULL) return FALSE;
        /* generate the entries file */
        generate_entries_file(relocation_table, symbol_table, entry_file);
        /* close the file stream */
        fclose(entry_file);
    }
    /* create the object file stream (the reason we don't check like in the other files is because the object file is mandatory) */
    object_file = create_file_stream(object_filename, WRITE_MODE);
    /* if the file could not be created, return FALSE */
    if (object_file == NULL) return FALSE;
    /* generate the object file */
    generate_object_file(code_image, data_image, object_file, num_of_command_words, dc);
    /* close the file stream */
    fclose(object_file);
    return TRUE;

}

void generate_externals_file(const SymbolTable *externals_table, FILE *extern_file) {
    int i;
    /* iterate over the externals table and write the symbol name and usage address to the file */
    for (i = 0; i < externals_table->size; i++) {
        fprintf(extern_file, "%s %d\n", externals_table->symbols[i]->name, externals_table->symbols[i]->address);
    }
}

void generate_entries_file(const SymbolTable *relocation_table, SymbolTable *symbol_table, FILE *entry_file) {
    int i;
    Symbol *relocation_symbol,*label_symbol;
    /* iterate every symbol in the relocation table  */
    for (i = 0; i < relocation_table->size; i++) {
        relocation_symbol = relocation_table->symbols[i];
        /* if the relocation delcaration is an entry, get the symbol from the symbol table and write it to the file */
        if (relocation_symbol->type == ENTRY) {
            label_symbol = get_symbol(symbol_table, relocation_symbol->name);
            fprintf(entry_file, "%s %d\n", label_symbol->name, label_symbol->address);
        }
    }
}

void generate_object_file(Word *code_image, Word *data_image, FILE *object_file, int num_of_command_words, int num_of_data_words) {
    /* write the number of words in the code image and the number of words in the data image as the first line of the file */
    fprintf(object_file, "%d %d", num_of_command_words, num_of_data_words);
    /* write the code image to the file */
    export_words_to_file(code_image + MEMORY_OFFSET, num_of_command_words, object_file);
    /* write the data image to the file */
    export_words_to_file(data_image, num_of_data_words, object_file);
}

void export_words_to_file(Word *words, int num_of_words, FILE *file) {
    int i;
    char base64_word[BASE64_WORD_LENGTH + 1] = {0};
    /* iterate over the words, encode each word to base64 and write it to the file */
    /* TODO: change back */
    for (i = 0; i < num_of_words; i++) {
        /* encode the word to base64 */
        word_to_base64(&words[i], base64_word);
        /* write the encoded word to the file */
        fprintf(file, "\n%s", base64_word);
    }
}
