#include <malloc.h>
#include <string.h>
#include "symbol_table.h"
#include "consts.h"
#include "instruction_handling.h"
#include "memory_wrappers.h"

SymbolTable *create_symbol_table() {
    SymbolTable *table;
    /* allocate memory for the table */
    table = (SymbolTable *) safe_malloc(sizeof(SymbolTable));
    /* initialize the table */
    table->capacity = INITIAL_SYMBOL_TABLE_CAPACITY;
    table->size = 0;
    /* allocate memory for the symbols array */
    table->symbols = (Symbol **) safe_malloc(sizeof(Symbol *) * table->capacity);
    return table;
}

Symbol *create_symbol(char *name, int address, InstructionType type) {
    /* allocate memory for the symbol */
    Symbol *symbol = (Symbol *) safe_malloc(sizeof(Symbol));
    /* allocate memory for the name */
    symbol->name = (char *) safe_malloc(strlen(name) + 1);
    /* initialize the symbol */
    strcpy(symbol->name, name);
    symbol->address = address;
    symbol->type = type;
    return symbol;
}

Bool add_symbol(SymbolTable *table, Symbol *symbol) {
    /* if the table is full, increase its capacity */
    if (table->size == table->capacity) {
        table->capacity += INITIAL_SYMBOL_TABLE_CAPACITY;
        table->symbols = (Symbol **) safe_realloc(table->symbols, table->capacity * sizeof(Symbol *));
    }
    /* add the symbol to the table */
    table->symbols[table->size] = symbol;
    table->size++;
    return TRUE;
}

void free_symbol(Symbol *symbol) {
    free(symbol->name);
    free(symbol);
}

void free_symbol_table(SymbolTable *table) {
    int i;
    for (i = 0; i < table->size; i++) {
        /* free each symbol in the table */
        free_symbol(table->symbols[i]);
    }
    free(table->symbols);
    free(table);
}

Symbol *get_symbol(SymbolTable *table, char *name) {
    int i;
    Symbol *symbol = NULL;
    if(table == NULL || name == NULL) {
        return NULL;
    }
    for (i = 0; i < table->size; i++) {
        symbol = table->symbols[i];
        /* if the symbol's name matches the given name, return it */
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
    }
    /* if the symbol was not found, return NULL */
    return NULL;
}

int count_symbols_by_type(SymbolTable *table, InstructionType type) {
    int i, count = 0;
    if(table == NULL) {
        return 0;
    }
    for (i = 0; i < table->size; i++) {
        /* if the symbol's type matches the given type, increment the counter */
        if (table->symbols[i]->type == type) {
            count++;
        }
    }
    return count;
}
