cmake_minimum_required(VERSION 3.24)
project(openu_assembler_project C)

set(CMAKE_C_STANDARD 90)

add_executable(openu_assembler_project assembler.c preprocess.c preprocess.h)
