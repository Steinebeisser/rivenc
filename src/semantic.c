//
// Created by Geisthardt on 22.05.2025.
//

#include "semantic.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Semantic_Context *create_semantic() {
        Semantic_Context *semantic_context = malloc(sizeof(Semantic_Context));
        if (!semantic_context) {
                alloc_error();
                exit(1);
                return NULL;
        }

        semantic_context->symbols = NULL;

        Scope *global_scope = malloc(sizeof(Scope));
        if (!global_scope) {
                free(semantic_context);
                alloc_error();
                exit(1);
        }

        *global_scope = (Scope) {
                .level = 0,
                .name = "GLOBAL",
                .parent = NULL,
                .symbols = NULL,
        };

        semantic_context->current_scope = global_scope;

        return semantic_context;
}

bool analyse_semantic(Ast_Node *program_node, Semantic_Context *semantic_context) {
        if (program_node->type != NODE_PROGRAM) {
                fprintf(stderr, "Input wrong node for Semantic Analysis, Expected Program got `%s`", node_type_name(program_node->type));
                exit(1);
                return false;
        }

        for (size_t i = 0; i < program_node->data.program.count; ++i) {
                Ast_Node *child = program_node->data.program.children[i];
                if (!analyse_node(child, semantic_context)) {
                        fprintf(stderr, "Failed to analyse node of type %s", node_type_name(child->type));
                        exit(1);
                        return false;
                }
        }
        return true;
}
bool analyse_node(Ast_Node *node, Semantic_Context *semantic_context) {
        if (!node) {
                fprintf(stderr, "tried analysing node, but passed NULL");
                return false;
        }

        switch (node->type) {
                case NODE_VARIABLE_DECLARATION: {
                        return analyse_variable_declaration(node, semantic_context);
                } break;
                case NODE_BINARY_EXPRESSION: {
                        return analyse_binary_expression(node, semantic_context);
                } break;
                case NODE_NUMBER: {
                        return analyse_number(node, semantic_context);
                } break;
                case NODE_IDENTIFIER: {
                        return analyse_identifier(node, semantic_context);
                } break;
                default: {
                        fprintf(stderr, "received unknown node type for semantic analysis: %s", node_type_name(node->type));
                        exit(1);
                };
        }
        return true;
}

void type_error(Ast_Node *node, Node_Type type) {
        if (node->type != type) {
                fprintf(stderr, "Got wrong node type, exprected %s, got %s", node_type_name(type), node_type_name(node->type));
                exit(1);
        }
}

Symbol *find_symbol(char *type_identifier, Semantic_Context *semantic_context) {
        if (!semantic_context) {
                fprintf(stderr, "No Semantic Context passed\n");
                exit(1);
        }
        Scope *scope = semantic_context->current_scope;

        printf("FINISNG SYMBOL: %s\n", type_identifier);
        while (scope) {
                Symbol *symbol = scope->symbols;
                while (symbol) {
                        if (strcmp(symbol->name, type_identifier) == 0) {
                                return symbol;
                        }
                        symbol = symbol->next;
                }
                scope = scope->parent;
        }

        return NULL;
}

Symbol *add_symbol(char *type_identifier, Variable_Type type, Semantic_Context *semantic_context) {
        if (!semantic_context) {
                fprintf(stderr, "No Semantic Context passed\n");
                exit(1);
        }
        Scope *scope = semantic_context->current_scope;

        Symbol *symbol = malloc(sizeof(Symbol));
        if (!symbol) {
                alloc_error();
                exit(1);
        }

        *symbol = (Symbol) {
                .name = type_identifier,
                .scope = scope,
                .type = type,
                .next = scope->symbols,
        };

        scope->symbols = symbol;

        return symbol;
}

bool analyse_variable_declaration(Ast_Node * node, Semantic_Context *semantic_context) {
        if (node->type != NODE_VARIABLE_DECLARATION) {
                type_error(node, NODE_VARIABLE_DECLARATION);
                exit(1);
        }

        Ast_Node *identifier_node = node->data.variable_declaration.identifier;
        char *type_identifier = identifier_node->data.identifier.name;

        Symbol *declared_symbol;
        if ((declared_symbol = find_symbol(type_identifier, semantic_context)) != NULL) {
                if (declared_symbol->type != node->data.variable_declaration.type) {
                        fprintf(stderr, "Tried to redeclare `%s` with different type (old: %s (Line: %d, Col: %d), new %s (Line: %llu, Col %llu))\n", type_identifier, variable_type_name(declared_symbol->type), 1, 1, variable_type_name(node->data.variable_declaration.type), node->location.row, node->location.col);
                        exit(-1);
                }
                fprintf(stderr, "WARNIGN: redeclared `%s` of Type `%s` at Line %llu Col %llu, initally declated at %llu %llu\n", type_identifier, variable_type_name(declared_symbol->type), identifier_node->location.row, identifier_node->location.col, 1, 1);
                // for now exit, later go through, maybe
                exit(1);
        }
        add_symbol(type_identifier, node->data.variable_declaration.type, semantic_context);

        if (!node->data.variable_declaration.initializer) {
                return true;
        }

        Ast_Node *initializer = node->data.variable_declaration.initializer;

        if (!analyse_node(initializer, semantic_context)) {
                fprintf(stderr, "failed to analyse initializer\n");
                exit(1);
        }
        return true;
}

bool analyse_binary_expression(Ast_Node * node, Semantic_Context *semantic_context) {

        printf("ANALYSING BINARY EXPRESSIOnm\n");

        if (node->type != NODE_BINARY_EXPRESSION) {
                type_error(node, NODE_BINARY_EXPRESSION);
                exit(1);
        }

        if (!analyse_node(node->data.binary_expression.left, semantic_context)) {
                return false;
        }

        if (!analyse_node(node->data.binary_expression.right, semantic_context)) {
                return false;
        }

        char *op = node->data.binary_expression.operator;

        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {

                Ast_Node *left = node->data.binary_expression.left;
                Ast_Node *right = node->data.binary_expression.right;

                if (left->type == NODE_IDENTIFIER) {
                        Symbol *symbol = find_symbol(left->data.identifier.name, semantic_context);
                        if (!symbol) {
                                return false;
                        }

                        if (symbol->type != VAR_TYPE_INT && symbol->type != VAR_TYPE_UINT && symbol->type != VAR_TYPE_FLOAT) {
                                return false;
                        }
                } else if (left->type != NODE_NUMBER && left->type != NODE_BINARY_EXPRESSION) {
                        return false;
                }

                if (right->type == NODE_IDENTIFIER) {
                        Symbol *symbol = find_symbol(right->data.identifier.name, semantic_context);
                        if (!symbol) {
                                return false;
                        }

                        if (symbol->type != VAR_TYPE_INT && symbol->type != VAR_TYPE_UINT && symbol->type != VAR_TYPE_FLOAT) {
                                return false;
                        }
                } else if (right->type != NODE_NUMBER && right->type != NODE_BINARY_EXPRESSION) {
                        return false;
                }
        }

        return true;
}

bool analyse_identifier(Ast_Node * node, Semantic_Context *semantic_context) {
        if (node->type != NODE_IDENTIFIER) {
                type_error(node, NODE_IDENTIFIER);
                exit(1);
        }

        printf("ANALYSING IDENTIFIER\n");
        Symbol *symbol = find_symbol(node->data.identifier.name, semantic_context);
        if (!symbol) {
                fprintf(stderr, "Undeclared identifier '%s' at Line %llu Col %llu\n",
                        node->data.identifier.name,
                        node->location.row, node->location.col);
                return false;
        }

        if (symbol->type != VAR_TYPE_INT && symbol->type != VAR_TYPE_UINT && symbol->type != VAR_TYPE_FLOAT) {
                fprintf(stderr, "Invalid operand type for operator '%s': left operand must be numeric\n", "`Placeholder`");
                return false;
        }
        return true;
}
bool analyse_number(Ast_Node * node, Semantic_Context *semantic_context) {
        if (node->type != NODE_NUMBER) {
                type_error(node, NODE_NUMBER);
                exit(1);
        }

        printf("ANALYSING NUMBER\n");

        return true;
}

