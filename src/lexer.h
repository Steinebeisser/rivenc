//
// Created by Geisthardt on 19.05.2025.
//

#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
#include <stdint.h>

typedef enum {
        TOKEN_END = 0,
        TOKEN_IDENTIFIER,    // Variable names, function names, etc.
        TOKEN_NUMBER,        // Numeric literals
        TOKEN_STRING,        // String literals
        TOKEN_KEYWORD,       // Reserved words (if, while, return, etc.)
        TOKEN_OPERATOR,      // +, -, *, /, etc.
        TOKEN_DELIMITER,     // Punctuation like ;, (, ), {, }, etc.
        TOKEN_COMMENT,       // Comment, starting with | ending with |
} Token_Kind;

typedef struct {
        const char *file_path;
        size_t row;
        size_t col;
} Location;

typedef struct {
        Token_Kind kind;
        const char *text;
        size_t text_len;
        Location location;
} Token;

typedef struct {
        const char *content;
        size_t content_length;
        size_t cursor;
        size_t line;
        size_t bol;
} Lexer;

typedef struct {
        const char *keyword;
        uint8_t len;
} Keyword;


Lexer lexer_create(const char *content, size_t content_length);

void lexer_move_right(Lexer *l);

Token lexer_next(Lexer *l);

char *token_kind_name(Token_Kind kind);

Token *collect_tokens(Lexer *l, size_t *out_count);

#endif //LEXER_H
