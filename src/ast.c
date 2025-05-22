//
// Created by Geisthardt on 21.05.2025.
//

#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_node_recursive(Ast_Node *node, size_t indent_level);


void alloc_error() {
        fprintf(stderr, "Failed to allocate memory for AST\n");
        exit(99);
}

Ast_Node create_program_node() {
        Ast_Node node = {
                .type = NODE_PROGRAM,
                .data = {.program =  {}}
        };

        return node;
}

Ast_Node *create_identifier_node(char *identifier_name, Location loc) {
        Ast_Node *node = malloc(sizeof(Ast_Node));
        if (!node) {
                alloc_error();
                return NULL;
        }
        *node = (Ast_Node){
                .type = NODE_IDENTIFIER,
                .data = {
                        .identifier = {
                                .name = identifier_name
                        }
                },
                .location = loc
        };

        return node;
}

Ast_Node *create_variable_declaration_node(char *type_name, Ast_Node *identifier, Ast_Node *initializer, Location loc) {
        Ast_Node *node = malloc(sizeof(Ast_Node));
        if (!node) {
                alloc_error();
                return NULL;
        }
        *node = (Ast_Node){
                .type = NODE_VARIABLE_DECLARATION,
                .data = {
                        .variable_declaration = {
                                .identifier = identifier,
                                .initializer = initializer
                        }
                },
                .location = loc,
        };

        if (strcmp(type_name, "int") == 0 ) { node->type = VAR_TYPE_INT; }
        else if (strcmp(type_name, "uint") == 0) { node->type = VAR_TYPE_UINT; }
        else if (strcmp(type_name, "float") == 0) { node->type = VAR_TYPE_FLOAT; }

        if (initializer) {
                initializer->parent = node;
        }

        return node;
}

Ast_Node *create_binary_expression_node(char *operator, Ast_Node *left, Ast_Node *right, Location loc) {
        Ast_Node *node = malloc(sizeof(Ast_Node));
        if (!node) {
                alloc_error();
                return NULL;
        }
        *node = (Ast_Node) {
                .type = NODE_BINARY_EXPRESSION,
                .data = {
                        .binary_expression = {
                                .left = left,
                                .right = right,
                                .operator = operator
                        }
                },
                .location = loc,
        };

        return node;
}

Ast_Node *create_number_node(size_t value, Location loc) {
        Ast_Node *node = malloc(sizeof(Ast_Node));
        if (!node) {
                alloc_error();
                return NULL;
        }
        *node = (Ast_Node){
                .type = NODE_NUMBER,
                .data = {
                        .number = {
                                .value = value
                        }
                },
                .location = loc
        };

        return node;
}

bool add_child_to_program(Ast_Node *program_node, Ast_Node *child) {
        if (program_node->type != NODE_PROGRAM) {
                fprintf(stderr, "Tried adding child to program, but passed program is of kind, %s", node_type_name(program_node->type));
                exit(1);
        }

        program_node->data.program.count += 1;
        Ast_Node **new_children = realloc(program_node->data.program.children, sizeof(Ast_Node) * program_node->data.program.count);
        if (!new_children) {
                free(new_children);
                alloc_error();
                return false;
        }

        program_node->data.program.children = new_children;
        program_node->data.program.children[program_node->data.program.count - 1] = child;

        child->parent = program_node;

        return true;
}

bool free_node(Ast_Node *node) {


}

char *node_type_name(Node_Type type) {
        switch (type) {
                case NODE_PROGRAM: return "program node";
                case NODE_VARIABLE_DECLARATION: return "variable declaration";
                case NODE_BINARY_EXPRESSION: return "binary expression";
                case NODE_NUMBER: return "number";
                case NODE_IDENTIFIER: return "identifier";
                default: return "unknown";
        }
}

void print_ast(Ast_Node *node) {
        print_node_recursive(node, 0);
}

size_t indentation_width = 3;

void print_node_recursive(Ast_Node *node, size_t indent_level) {
        if (!node) {
                return;
        }

        char indentation[indent_level * indentation_width];
        memset(indentation, ' ', indent_level * indentation_width);
        indentation[indent_level * indentation_width - 1] = '\0';
        printf("%s", indentation);

        switch (node->type) {
                case NODE_PROGRAM: {
                        printf("PROGRAM NODE (%llu children)\n", node->data.program.count);
                        for (size_t i = 0; i < node->data.program.count; ++i) {
                                print_node_recursive(node->data.program.children[i], indent_level + 1);
                        }
                } break;
                case NODE_VARIABLE_DECLARATION: {
                        printf("VARIABLE DECLARATION: %s\n", variable_type_name(node->data.variable_declaration.type));
                        print_node_recursive(node->data.variable_declaration.identifier, indent_level + 1);
                        if (node->data.variable_declaration.initializer) {
                                print_node_recursive(node->data.variable_declaration.initializer, indent_level + 1);
                        }
                } break;
                case NODE_BINARY_EXPRESSION: {
                        printf("BINARY EXPRESSION (%s)\n", node->data.binary_expression.operator);
                        print_node_recursive(node->data.binary_expression.left, indent_level + 1);
                        print_node_recursive(node->data.binary_expression.right, indent_level + 1);
                } break;
                case NODE_NUMBER: {
                        printf("NUMBER: %llu\n", node->data.number.value);
                } break;
                case NODE_IDENTIFIER: {
                        printf("IDENTIFIER (%s)\n", node->data.identifier.name);
                } break;
                default: {
                        printf("UNKNOWN TYPE\n");
                }
        }
}

char *variable_type_name(Variable_Type type) {
       switch (type) {
               case VAR_TYPE_INT: { return "Int"; }
               case VAR_TYPE_UINT: { return "UInt"; }
               case VAR_TYPE_FLOAT: { return "Float"; }
               default: {
                       return "Unknown Type";
               }
       }
}
