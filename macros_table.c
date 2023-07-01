#include <malloc.h>
#include <string.h>
#include "macros_table.h"


static void free_macro_item(MacroItem *item);

MacroTable *create_macro_table(){
    MacroTable *table = (MacroTable*) malloc(sizeof(MacroTable));
    table->capacity = 0;
    table->size = 0;
    table->items = NULL;
    return table;
}

MacroItem *create_macro_item(char *name, char **value, int line_number, int value_size){
    MacroItem *item = (MacroItem*) malloc(sizeof(MacroItem));
    item->name = malloc(strlen(name) + 1);
    strcpy(item->name, name);
    item->value = value;
    item->line_number = line_number;
    item->value_size = value_size;
    return item;
}

Bool add_macro_item(MacroTable *table, MacroItem *item){
    if(table->size == table->capacity){
        table->capacity += MACRO_TABLE_CAPACITY;
        table->items = realloc(table->items,table->capacity * sizeof(MacroItem));
        if(table->items == NULL){
            return FALSE;
        }
    }
    table->items[table->size] = *item;
    table->size++;
    return TRUE;
}

void free_macro_item(MacroItem *item){
    free(item->name);
    free(item->value);
}


MacroItem *get_macro_item(MacroTable *table, char *name){
    int i;
    MacroItem *item = NULL;
    for(i=0;i<table->size;i++){
        item = &table->items[i];
        if(strcmp(item->name, name) == 0){
            return item;
        }
    }
    return NULL;
}

void free_macro_table(MacroTable *table){
    int i;
    for(i=0;i<table->size;i++){
        free_macro_item(&table->items[i]);
    }
    free(table->items);
    free(table);
}

Bool append_macro_item_value(MacroItem *item, char *value_to_append) {
    int i;
    char **new_value;

    if (item->value == NULL) {
        item->value = malloc(2 * sizeof(char *));
        if (item->value == NULL) {
            return FALSE;
        }
        item->value[0] = duplicate_string(value_to_append);
        item->value[1] = NULL;
    } else {
        for (i = 0; item->value[i] != NULL; i++)
            ;

        new_value = realloc(item->value, (i + 2) * sizeof(char *));
        if (new_value == NULL) {
            return FALSE;
        }

        item->value = new_value;
        item->value[i] = duplicate_string(value_to_append);
        item->value[i + 1] = NULL;
    }

    item->value_size += 1;
    return TRUE;
}
