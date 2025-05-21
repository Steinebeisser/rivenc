//
// Created by Geisthardt on 21.05.2025.
//

#include "parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *valid_types[] = {
        "int",
        "uint",
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
bool parser_parse(Parser *p) {
        while (p->current.kind != TOKEN_END) {
                if (p->current.kind == TOKEN_COMMENT) {
                        move_forward(p);
                        continue;
                }
                parse_statement(p);
        }
        return true;
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

void parse_statement(Parser *p) {
        // [type] identifier [statementtail]
        parse_type(p);
        expect(p, TOKEN_IDENTIFIER);
        move_forward(p);
        parse_statement_tail(p);
        printf("PARSED STATEMENT, HYPE\n");
}

void parse_statement_tail(Parser *p) {
        // ; | = [expression] ; | ( [parameterlist] ) [block]
        if (p->current.kind == TOKEN_DELIMITER && p->current.text_len == 1 && p->current.text[0] == ';') {
                move_forward(p);
                return;
        }

        if (p->current.kind == TOKEN_OPERATOR && p->current.text_len == 1 && p->current.text[0] == '=') {
                move_forward(p);
                parse_expression(p);
                expect(p, TOKEN_DELIMITER);
                if (p->current.text[0] != ';') {
                        fprintf(stderr, "Expected ';' after expression at %llu|%llu\n", p->current.location.row, p->current.location.col);
                        exit(1);
                }
                move_forward(p);
                return;
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


void parse_expression(Parser *p) {
        // [term] [Expression']
        parse_term(p);
        parse_expression_prime(p);
}

void parse_expression_prime(Parser *p) {
        if (p->current.kind != TOKEN_OPERATOR) {
                return;
        }

        if (p->current.text[0] == '+') {
                move_forward(p);
                parse_term(p);
                parse_expression_prime(p);
                return;
        }

        if (p->current.text[0] == '-') {
                move_forward(p);
                parse_term(p);
                parse_expression_prime(p);
                return;
        }
}

void parse_term(Parser *p) {
        // [factor] [term']
        parse_factor(p);
        parse_term_prime(p);
}

void parse_term_prime(Parser *p) {
        // * [factor] | / [factor] | epsilon
        if (p->current.text[0] == '*') {
                move_forward(p);
                parse_factor(p);
                parse_term_prime(p);
                return;
        }


        if (p->current.text[0] == '/') {
                move_forward(p);
                parse_factor(p);
                parse_term_prime(p);
                return;
        }

}

void parse_factor(Parser *p) {
        // number | [expression]
        if (p->current.kind == TOKEN_NUMBER) {
                char buffer[p->current.text_len + 1];

                memcpy(buffer, p->current.text, p->current.text_len);
                buffer[p->current.text_len] = '\0';

                char *endptr = NULL;
                long value = strtol(buffer, &endptr, 10);

                if (endptr != buffer + p->current.text_len) {
                        fprintf(stderr, "Invalid number: '%.*s'\n",
                                (int)p->current.text_len, p->current.text);
                        exit(1);
                }

                printf("NUMBER: %d\n", value);
                move_forward(p);
        } else {
                parse_expression(p);
        }
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


void parse_type(Parser *p) {
        expect(p, TOKEN_KEYWORD);

        if (is_type_keyword(&p->current)) {
                printf("GOT CORRECT TYPE\n");
                move_forward(p);
        } else {
                fprintf(stderr, "Expected a type keyword, got '%.*s' at Position: %llu|%llu, %.*s\n",
                        (int)p->current.text_len, p->current.text,
                        p->current.location.row, p->current.location.col,
                        (int) p->current.text_len, p->current.text);
                exit(1);
        }
}
//void parse_function_declaration() {
//        parse_type();
//        parse_identifier();
//        expect("(");
//        parse_parameter_list();
//        expect(")");
//        parse_block();
//}