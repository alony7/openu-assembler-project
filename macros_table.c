#include <malloc.h>
#include <string.h>
#include "macros_table.h"

MacroTable create_macro_table(){
    MacroTable table;
    table.capacity = 0;
    table.size = 0;
    table.items = NULL;
    return table;
}

MacroItem *create_macro_item(char *name, char **value, int row_number){
    MacroItem *item = malloc(sizeof(MacroItem));
    item->name = name;
    item->value = value;
    item->row_number = row_number;
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

MacroItem *remove_macro(MacroTable *table, char *macro_name){
    int i;
    MacroItem *item;
    for(i=0;i<table->size;i++){
        item = &table->items[i];
        if(strcmp(item->name, macro_name) == 0){
            free_macro_item(item);
            return item;
        }
    }
    return NULL;
}

MacroItem *get_macro_item(MacroTable *table, char *name){
    int i;
    MacroItem *item;
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
}

Bool append_macro_item_value(MacroItem *item, char *value){
    int i;
    char **new_value;
    if(item->value == NULL){
        item->value = malloc(sizeof(char*));
        if(item->value == NULL){
            return FALSE;
        }
        item->value[0] = NULL;
    }
    for(i=0;item->value[i] != NULL;i++);
    new_value = realloc(item->value,(i+2) * sizeof(char*));
    if(new_value == NULL){
        return FALSE;
    }
    item->value = new_value;
    item->value[i] = value;
    item->value[i+1] = NULL;
    return TRUE;
}