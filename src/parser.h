//
// Created by Geisthardt on 21.05.2025.
//

#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>


#include "lexer.h"

typedef struct {
        Token *tokens;
        size_t index;
        size_t count;
        Token current;
} Parser;

Parser parser_create(Token *tokens, size_t token_count);

bool parser_parse(Parser *p);


bool expect(Parser *p, Token_Kind kind);
void move_forward(Parser *p);


void parse_type(Parser *p);
void parse_statement(Parser *p);
void parse_statement_tail(Parser *p);
void parse_expression(Parser *p);
void parse_expression_prime(Parser *p);
void parse_term(Parser *p);
void parse_term_prime(Parser *p);
void parse_factor(Parser *p);

#endif //PARSER_H
