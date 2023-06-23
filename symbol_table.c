#include <malloc.h>
#include <string.h>
#include "symbol_table.h"
#include "consts.h"

SymbolTable create_symbol_table() {
    SymbolTable table;
    table.capacity = INITIAL_SYMBOL_TABLE_CAPACITY;
    table.size = 0;
    table.symbols = (Symbol *) malloc(sizeof(Symbol) * table.capacity);
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

void add_symbol(SymbolTable *table, Symbol *symbol) {
    if (table->size == table->capacity) {
        table->capacity += INITIAL_SYMBOL_TABLE_CAPACITY;
        table->symbols = (Symbol *) realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }
    table->symbols[table->size] = *symbol;
    table->size++;
}

void free_symbol(Symbol *symbol) {
    free(symbol->name);
    free(symbol);
}

void free_symbol_table(SymbolTable *table) {
    int i;
    for (i = 0; i < table->size; i++) {
        free_symbol(&table->symbols[i]);
    }
    free(table->symbols);
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

