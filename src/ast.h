//
// Created by Geisthardt on 21.05.2025.
//

#ifndef AST_H
#define AST_H
#include <stdbool.h>
#include <stddef.h>

#include "lexer.h"

typedef enum {
        NODE_PROGRAM,
        NODE_VARIABLE_DECLARATION,
        NODE_BINARY_EXPRESSION,
        NODE_NUMBER,
        NODE_IDENTIFIER,
} Node_Type;

typedef enum {
        VAR_TYPE_INT,
        VAR_TYPE_UINT,
        VAR_TYPE_FLOAT
} Variable_Type;

typedef struct Ast_Node {
        Node_Type type;
        Location location;
        struct Ast_Node *parent;
        union {
                struct {
                        size_t count;
                        struct Ast_Node **children;
                } program;
                struct {
                        Variable_Type type;
                        struct Ast_Node *identifier;
                        struct Ast_Node *initializer;
                } variable_declaration;

                struct {
                        char *operator;
                        struct Ast_Node *left;
                        struct Ast_Node *right;
                } binary_expression;

                struct {
                        size_t value;
                } number;

                struct {
                        char *name;
                } identifier;
        } data;
} Ast_Node;

Ast_Node create_program_node();
Ast_Node *create_identifier_node(char *identifier_name, Location loc);
Ast_Node *create_variable_declaration_node(char *type_name, Ast_Node *identifier, Ast_Node *initializer, Location loc);
Ast_Node *create_binary_expression_node(char *operator, Ast_Node *left, Ast_Node *right, Location loc);
Ast_Node *create_number_node(size_t value, Location loc);

bool add_child_to_program(Ast_Node *program_node, Ast_Node *child);

bool free_node(Ast_Node *node);

void alloc_error();

char *node_type_name(Node_Type type);

void print_ast(Ast_Node *node);

char *variable_type_name(Variable_Type type);

#endif //AST_H
