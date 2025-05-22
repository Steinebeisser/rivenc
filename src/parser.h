//
// Created by Geisthardt on 21.05.2025.
//

#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>


#include "ast.h"
#include "lexer.h"

typedef struct {
        Token *tokens;
        size_t index;
        size_t count;
        Token current;
} Parser;

Parser parser_create(Token *tokens, size_t token_count);

Ast_Node parser_parse(Parser *p);


bool expect(Parser *p, Token_Kind kind);
void move_forward(Parser *p);
char *get_current_text(Parser *p);


char *parse_type(Parser *p);
Ast_Node *parse_statement(Parser *p);
Ast_Node *parse_statement_tail(Parser *p, char *type_name, Ast_Node *identifier);
Ast_Node *parse_expression(Parser *p);
Ast_Node *parse_expression_prime(Parser *p, Ast_Node *left);
Ast_Node *parse_term(Parser *p);
Ast_Node *parse_term_prime(Parser *p, Ast_Node *left);
Ast_Node *parse_factor(Parser *p);

#endif //PARSER_H
