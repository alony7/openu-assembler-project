#pragma once

#define ASSIGN_LEFT_LOGICAL_AND(left, right) (left = (right) && left)

#define LAST_CHAR(string) (string[strlen(string) - 1])

typedef enum Bool {
    FALSE = 0,
    TRUE = 1
} Bool;

typedef enum LogLevel {
    INFO = 0,
    WARN = 1,
    ERROR = 2
} LogLevel;
