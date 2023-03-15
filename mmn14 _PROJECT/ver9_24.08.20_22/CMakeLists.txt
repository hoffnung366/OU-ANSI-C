cmake_minimum_required(VERSION 3.17)
project(mmn14_project C)

set(CMAKE_C_STANDARD 90)

add_executable(mmn14_project main.c first_read.c second_read.c word_fill.c symbol_table.c memory_map.c data.h errors.c)