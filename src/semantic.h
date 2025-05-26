//
// Created by Geisthardt on 22.05.2025.
//

#ifndef SEMANTIC_H
#define SEMANTIC_H
#include <stdbool.h>

#include "ast.h"

typedef struct Scope {
        int level;
        char *name;
        struct Scope *parent;
        struct Symbol *symbols;
} Scope;

typedef struct Symbol {
        char *name;
        Variable_Type type;
        union {
                int int_value; //int
                uint32_t uint_value; //uint
                float float_value;
                char *string_value; //string
        };
        struct Symbol *next;
        Scope *scope;
} Symbol;

typedef struct {
        Symbol **symbols;
        Scope *current_scope;
} Semantic_Context;


Semantic_Context *create_semantic();

Symbol *find_symbol(char *type_identifier, Semantic_Context *semantic_context);
Symbol *add_symbol(char *type_identifier, Variable_Type type, Semantic_Context *semantic_context);


bool analyse_semantic(Ast_Node *program_node, Semantic_Context *semantic_context);

bool analyse_node(Ast_Node *node, Semantic_Context *semantic_context);
bool analyse_identifier(Ast_Node * node, Semantic_Context *semantic_context);
bool analyse_number(Ast_Node * node, Semantic_Context *semantic_context);
bool analyse_variable_declaration(Ast_Node * node, Semantic_Context *semantic_context);
bool analyse_binary_expression(Ast_Node * node, Semantic_Context *semantic_context);

#endif //SEMANTIC_H
