#pragma once

#define CHECK_AND_UPDATE_SUCCESS(is_success, instruction) (is_success = (instruction) && is_success)

typedef enum Bool {
    FALSE = 0,
    TRUE = 1
} Bool;

