#pragma once
#include "consts.h"
#include "utils.h"


MacroTable *create_macro_table();

Bool add_macro_item(MacroTable *table, MacroItem *item);

MacroItem *get_macro_item(MacroTable *table, char *name);

void free_macro_table(MacroTable *table);

MacroItem *create_macro_item(char *name, char **value, int line_number, int value_size);

Bool append_macro_item_value(MacroItem *item, char *value_to_append);

void free_macro_item_value(char **value);