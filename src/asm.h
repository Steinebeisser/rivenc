//
// Created by Geisthardt on 28.05.2025.
//

#ifndef ASM_H
#define ASM_H
#include <stdio.h>

#include "ast.h"

char *generate_asm(Ast_Node *program_node); //returns filepath
bool compile_asm_with_fasm(char *filepath);

void generate_node(FILE *fd, Ast_Node *node);


#endif //ASM_H
