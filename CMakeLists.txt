cmake_minimum_required(VERSION 3.24)
project(openu_assembler_project C)

set(CMAKE_C_STANDARD 90)

add_executable(openu_assembler_project assembler.c pre_process.c pre_process.h macros_table.c macros_table.h consts.h utils.c input_utils.c input_utils.h)
