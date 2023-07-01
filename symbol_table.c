#include <malloc.h>
#include <string.h>
#include "symbol_table.h"
#include "consts.h"
#include "instruction_handling.h"

SymbolTable *create_symbol_table() {
    SymbolTable *table;
    table = (SymbolTable *) malloc(sizeof(SymbolTable));
    table->capacity = INITIAL_SYMBOL_TABLE_CAPACITY;
    table->size = 0;
    table->symbols = (Symbol *) malloc(sizeof(Symbol) * table->capacity);
    return table;
}

Symbol *create_symbol(char *name, int address, InstructionType type) {
    Symbol *symbol = (Symbol *) malloc(sizeof(Symbol));
    symbol->name = (char *) malloc(strlen(name) + 1);
    strcpy(symbol->name, name);
    symbol->address = address;
    symbol->type = type;
    return symbol;
}

Bool add_symbol(SymbolTable *table, Symbol *symbol) {
    if (table->size == table->capacity) {
        table->capacity += INITIAL_SYMBOL_TABLE_CAPACITY;
        table->symbols = (Symbol *) realloc(table->symbols, table->capacity * sizeof(Symbol));
        if (table->symbols == NULL) {
            return FALSE;
        }
    }
    table->symbols[table->size] = *symbol;
    table->size++;
    return TRUE;
}

void free_symbol(Symbol *symbol) {
    free(symbol->name);
}

void free_symbol_table(SymbolTable *table) {
    int i;
    for (i = 0; i < table->size; i++) {
        free_symbol(&table->symbols[i]);
    }
    free(table->symbols);
    free(table);
}

Symbol *get_symbol(SymbolTable *table, char *name) {
    int i;
    Symbol *symbol = NULL;
    for (i = 0; i < table->size; i++) {
        symbol = &table->symbols[i];
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
    }
    return NULL;
}

int count_symbols_by_type(SymbolTable *table, InstructionType type) {
    int i, count = 0;
    for (i = 0; i < table->size; i++) {
        if (table->symbols[i].type == type) {
            count++;
        }
    }
    return count;
}
