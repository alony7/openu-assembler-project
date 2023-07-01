#include "output_generator.h"

void export_words_to_file(Word *words, int num_of_words, FILE *file);

void generate_object_file(Word *code_image, Word *data_image, FILE *object_file, int num_of_command_words, int num_of_data_words);

void generate_entries_file(const SymbolTable *relocation_table, SymbolTable *symbol_table, FILE *entry_file);

void generate_externals_file(const SymbolTable *externals_table, FILE *extern_file);

Bool generate_output_files(char *object_filename, char *entry_filename, char *extern_filename, SymbolTable *relocation_table, SymbolTable *symbol_table, SymbolTable *externals_table, Word *code_image, Word *data_image, int dc, int ic) {
    FILE *object_file, *entry_file, *extern_file;

    int num_of_command_words = ic - MEMORY_OFFSET, i;
    if (count_symbols_by_type(relocation_table, EXTERN)) {
        extern_file = create_file_stream(extern_filename, WRITE_MODE);
        if (extern_file == NULL) return FALSE;
        generate_externals_file(externals_table, extern_file);

    }
    if (count_symbols_by_type(relocation_table, ENTRY)) {
        entry_file = create_file_stream(entry_filename, WRITE_MODE);
        if (entry_file == NULL) return FALSE;
        generate_entries_file(relocation_table, symbol_table, entry_file);
    }
    object_file = create_file_stream(object_filename, WRITE_MODE);
    if (object_file == NULL) return FALSE;
    generate_object_file(code_image, data_image, object_file, num_of_command_words, dc);
    fclose(object_file);
    return TRUE;

}

void generate_externals_file(const SymbolTable *externals_table, FILE *extern_file) {
    int i;
    for (i = 0; i < externals_table->size; i++) {
        fprintf(extern_file, "%s %d\n", externals_table->symbols[i].name, externals_table->symbols[i].address);
    }
}

void generate_entries_file(const SymbolTable *relocation_table, SymbolTable *symbol_table, FILE *entry_file) {
    int i;
    Symbol *relocation_symbol;
    Symbol *label_symbol;
    for (i = 0; i < relocation_table->size; i++) {
        relocation_symbol = &relocation_table->symbols[i];
        if (relocation_symbol->type == ENTRY) {
            label_symbol = get_symbol(symbol_table, relocation_symbol->name);
            fprintf(entry_file, "%s %d\n", label_symbol->name, label_symbol->address);
        }
    }
}

void generate_object_file(Word *code_image, Word *data_image, FILE *object_file, int num_of_command_words, int num_of_data_words) {
    fprintf(object_file, "%d %d", num_of_command_words, num_of_data_words);
    export_words_to_file(code_image + MEMORY_OFFSET, num_of_command_words, object_file);
    export_words_to_file(data_image, num_of_data_words, object_file);
}

void export_words_to_file(Word *words, int num_of_words, FILE *file) {
    int i;
    char base64_word[BASE64_WORD_LENGTH + 1] = "";
    for (i = 0; i < num_of_words; i++) {
        word_to_base64(&words[i], base64_word);
        fprintf(file, "\n%s", base64_word);
    }
}
