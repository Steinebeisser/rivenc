//
// Created by Geisthardt on 21.05.2025.
//

#include "parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *valid_types[] = {
        "int", "uint", "float"
};

#define NUM_VALID_TYPES (sizeof(valid_types) / sizeof(valid_types[0]))


Parser parser_create(Token *tokens, size_t token_count) {
        Parser parser = {
                .tokens = tokens,
                .count = token_count,
                .current = tokens[0],
                .index = 0
        };

        return parser;
}
Ast_Node parser_parse(Parser *p) {
        Ast_Node program_node = create_program_node();
        while (p->current.kind != TOKEN_END) {
                if (p->current.kind == TOKEN_COMMENT) {
                        move_forward(p);
                        continue;
                }
                Ast_Node *statement_node = parse_statement(p);
                if (!statement_node) {
                        fprintf(stderr, "Failed to create AST for current Tokens\n");
                        exit(69);
                }
                add_child_to_program(&program_node, statement_node);
        }
        return program_node;
}

bool expect(Parser *p, Token_Kind kind) {
        if (p->current.kind != kind) {
                fprintf(stderr, "Expected %s but got %s, at Position: '%llu'|'%llu', '%.*s'\n", token_kind_name(kind), token_kind_name(p->current.kind), p->current.location.row, p->current.location.col, (int) p->current.text_len, p->current.text);
                exit(1);
        }
        return true;
}

void move_forward(Parser *p) {
        p->current = p->tokens[++p->index];
}

char *get_current_text(Parser *p) {
        char *current_text = malloc(p->current.text_len + 1);
        if (!current_text) {
                fprintf(stderr, "Failed to allocate memory\n");
                exit(420);
        }
        memcpy(current_text, p->current.text, p->current.text_len);
        current_text[p->current.text_len] = '\0';

        return current_text;
}

Ast_Node *parse_statement(Parser *p) {
        // [type] identifier [statementtail]

        char *type_name = parse_type(p);
        expect(p, TOKEN_IDENTIFIER);
        char *identifier_name = get_current_text(p);
        Ast_Node *identifier = create_identifier_node(identifier_name, p->current.location);
        move_forward(p);
        Ast_Node *statement_node = parse_statement_tail(p, type_name, identifier);
        printf("PARSED STATEMENT, HYPE\n");
        return statement_node;
}

Ast_Node *parse_statement_tail(Parser *p, char *type_name, Ast_Node *identifier) {
        // ; | = [expression] ; | ( [parameterlist] ) [block]
        if (p->current.kind == TOKEN_DELIMITER && p->current.text_len == 1 && p->current.text[0] == ';') {
                move_forward(p);
                return create_variable_declaration_node(type_name, identifier, NULL, p->current.location);
        }

        if (p->current.kind == TOKEN_OPERATOR && p->current.text_len == 1 && p->current.text[0] == '=') {
                move_forward(p);
                Ast_Node *initializer = parse_expression(p);
                expect(p, TOKEN_DELIMITER);
                if (p->current.text[0] != ';') {
                        fprintf(stderr, "Expected ';' after expression at %llu|%llu\n", p->current.location.row, p->current.location.col);
                        exit(1);
                }
                move_forward(p);
                return create_variable_declaration_node(type_name, identifier, initializer, p->current.location);
        }

        //if (p->current.kind == TOKEN_DELIMITER && p->current.text_len == 1 && p->current.text[0] == '(') {
        //        move_forward(p);
        //        parse_parameter_list(p);
        //        expect(p, TOKEN_DELIMITER);
        //        if (p->current.text_len != 1 || p->current.text[0] != ')') {
        //                fprintf(stderr, "Expected ')' after parameter list at %llu|%llu\n", p->current.location.row, p->current.location.col);
        //                exit(1);
        //        }
        //        move_forward(p);

        //        parse_block(p);
        //        return;
        //}

        fprintf(stderr, "Expected '=', ';', or '(' after identifier at %llu|%llu, but got '%.*s'\n",
                p->current.location.row, p->current.location.col,
                (int)p->current.text_len, p->current.text);
        exit(1);
}


Ast_Node *parse_expression(Parser *p) {
        // [term] [Expression']
        Ast_Node *left = parse_term(p);
        return parse_expression_prime(p, left);
}

Ast_Node *parse_expression_prime(Parser *p, Ast_Node *left) {
        if (p->current.kind != TOKEN_OPERATOR) {
                return left;
        }

        if (p->current.text[0] == '+' || p->current.text[0] == '-') {
                char *operator = malloc(2);
                if (!operator) {
                        alloc_error();
                        return NULL;
                }
                operator[0] = p->current.text[0];
                operator[1] = '\0';
                move_forward(p);

                Ast_Node *right = parse_term(p);
                Ast_Node *binary_node = create_binary_expression_node(operator, left, right, p->current.location);

                return parse_expression_prime(p, binary_node);
        }

        return left;
}

Ast_Node *parse_term(Parser *p) {
        // [factor] [term']
        Ast_Node *left = parse_factor(p);
        return parse_term_prime(p, left);
}

Ast_Node *parse_term_prime(Parser *p, Ast_Node *left) {
        // * [factor] | / [factor] | epsilon
        if (p->current.text[0] == '/' || p->current.text[0] == '*') {
                char *operator = malloc(2);
                if (!operator) {
                        alloc_error();
                        return NULL;
                }
                operator[0] = p->current.text[0];
                operator[1] = '\0';

                move_forward(p);

                Ast_Node *right = parse_factor(p);
                Ast_Node *binary_node = create_binary_expression_node(operator, left, right, p->current.location);

                return parse_term_prime(p, binary_node);
        }

        return left;
}

Ast_Node *parse_factor(Parser *p) {
        // number | [expression]
        if (p->current.kind != TOKEN_NUMBER) {
                return parse_expression(p);
        }

        char buffer[p->current.text_len + 1];

        memcpy(buffer, p->current.text, p->current.text_len);
        buffer[p->current.text_len] = '\0';

        char *endptr = NULL;
        size_t value = strtol(buffer, &endptr, 10);

        if (endptr != buffer + p->current.text_len) {
                fprintf(stderr, "Invalid number: '%.*s'\n",
                        (int)p->current.text_len, p->current.text);
                exit(1);
        }

        Ast_Node *number_node = create_number_node(value, p->current.location);
        move_forward(p);
        return number_node;
}

bool is_type_keyword(Token *t) {
        for (size_t i = 0; i < NUM_VALID_TYPES; ++i) {
                const char *type_name = valid_types[i];
                size_t len = strlen(type_name);

                if (t->text_len == len && strncmp(t->text, type_name, len) == 0) {
                        return true;
                }
        }
        return false;
}


char *parse_type(Parser *p) {
        expect(p, TOKEN_KEYWORD);

        if (!is_type_keyword(&p->current)) {
                fprintf(stderr, "Expected a type keyword, got '%.*s' at Position: %llu|%llu, %.*s\n",
                        (int)p->current.text_len, p->current.text,
                        p->current.location.row, p->current.location.col,
                        (int) p->current.text_len, p->current.text);
                exit(1);
        }

        char *type_name = get_current_text(p);
        move_forward(p);
        return type_name;
}
//void parse_function_declaration() {
//        parse_type();
//        parse_identifier();
//        expect("(");
//        parse_parameter_list();
//        expect(")");
//        parse_block();
//}