cmake_minimum_required(VERSION 3.24)
project(openu_assembler_project C)

set(CMAKE_C_STANDARD 90)

add_executable(openu_assembler_project assembler.c pre_process.c pre_process.h macros_table.c macros_table.h consts.h utils.c io_parsers.c io_parsers.h first_step.c first_step.h symbol_table.c symbol_table.h instruction_handling.c instruction_handling.h error.c error.h util_types.h second_step.c second_step.h output_generator.c output_generator.h memory_wrappers.c memory_wrappers.h)
