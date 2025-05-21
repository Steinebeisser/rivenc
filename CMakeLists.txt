cmake_minimum_required(VERSION 3.30)
project(rivenc C)

set(CMAKE_C_STANDARD 11)

add_executable(rivenc rivenc.c
        src/lexer.c
        src/lexer.h
        src/utils.c
        src/utils.h
        src/parser.c
        src/parser.h)
