cmake_minimum_required(VERSION 3.26)
project(pascal2C C)

set(CMAKE_C_STANDARD 11)

add_executable(pascal2C main.c
        ast.h
        semantic.c
        ast.c
        ast.c)
