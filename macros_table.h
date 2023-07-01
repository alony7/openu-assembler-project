#include "consts.h"
#include "utils.h"
#pragma once


MacroTable *create_macro_table();

Bool add_macro_item(MacroTable *table, MacroItem *item);

MacroItem *remove_macro(MacroTable *table, char *macro_name);

MacroItem *get_macro_item(MacroTable *table, char *name);

void free_macro_table(MacroTable *table);

void free_macro_item(MacroItem *item);

MacroItem *create_macro_item(char *name, char **value, int line_number, int value_size);

Bool append_macro_item_value(MacroItem *item, char *value_to_append);