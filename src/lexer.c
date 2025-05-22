//
// Created by Geisthardt on 19.05.2025.
//

#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *raw_keywords[] = {
        "if", "while", "where", "int", "printf", "float"
};

#define KEYWORD_AMOUNT sizeof(raw_keywords) / sizeof(raw_keywords[0])

Keyword keywords[KEYWORD_AMOUNT];

void init_keywords() {
        for (size_t i = 0; i < KEYWORD_AMOUNT; ++i) {
                keywords[i].keyword = raw_keywords[i];
                keywords[i].len = strlen(raw_keywords[i]);
        }
}

Lexer lexer_create(const char *content, size_t content_length) {
        Lexer lexer = {0};

        lexer.content = content;
        lexer.content_length = content_length;

        init_keywords();

        return lexer;
}
bool is_eof(char c) {
        return c == '\0';
}

bool is_whitespace(char c) {
        return c == ' ' ||
                c == '\t' ||
                c == '\n' ||
                c == '\r';
}

bool is_line_end(char c) {
        return c == '\r' ||
                c == '\n';
}

void lexer_move_right(Lexer *l) {
        //move over whitespaces to first character
        while (l->cursor < l->content_length && is_whitespace(l->content[l->cursor])) {

                if (l->content[l->cursor] == '\n') {
                        l->line++;
                        l->bol = l->cursor + 1;
                }
                l->cursor++;
        }
}

bool is_string_start_end(char c) {
        return c == '\"' || c == '\'';
}

bool is_symbol_start(char c) {
        return isalpha(c) ||c == '_';
}

bool is_symbol(char c) {
        return isalnum(c) || c == '_';
}

bool is_seperator(char c) {
        return strchr("(){}[],;:", c) != NULL;
}

bool is_operator(char c) {
        return strchr("+-*/%=<>!&^~?", c) != NULL;
}

bool is_number(char c) {
        return isdigit(c);
}

bool is_comment_start(char c) {
        return c == '|';
}

bool is_comment_end(char c) {
        return c == '>';
}

Token lexer_next(Lexer *l) {
        lexer_move_right(l);

        Token token = {
                .text = &l->content[l->cursor],
                .location = {
                        .row = l->line +1,
                        .col = l->cursor - l->bol + 1
                    }
        };

        if (l->cursor >= l->content_length) {
                printf("EOF\n");
                return token;
        }


        //KEYWORDS
        Keyword potential_keywords[KEYWORD_AMOUNT];
        size_t potential_keyword_amount = 0;

        for (size_t i = 0; i < KEYWORD_AMOUNT; ++i) {
                if (keywords[i].keyword[0] == l->content[l->cursor]) {
                        potential_keywords[potential_keyword_amount] = keywords[i];
                        potential_keyword_amount += 1;
                }
        }
        size_t character_iteration = 1;
        while (potential_keyword_amount > 0) {
                if (l->cursor + character_iteration > l->content_length) {
                        break;
                }
                for (size_t i = 0; i < potential_keyword_amount; ++i) {
                        if (potential_keywords[i].keyword[character_iteration] != l->content[l->cursor + character_iteration]) {
                                potential_keywords[i] = potential_keywords[potential_keyword_amount - 1];
                                potential_keyword_amount -= 1;
                                i -= 1;
                                continue;
                        }

                        if (character_iteration + 1 == potential_keywords[i].len) {
                                if (is_whitespace(l->content[l->cursor + character_iteration + 1]) ||
                                           is_eof(l->content[l->cursor + character_iteration + 1]) ||
                                           is_comment_start(l->content[l->cursor + character_iteration + 1]) ||
                                           is_seperator(l->content[l->cursor + character_iteration + 1]) ||
                                           is_string_start_end(l->content[l->cursor + character_iteration + 1]) ||
                                           is_operator(l->content[l->cursor + character_iteration + 1])) {
                                        token.kind = TOKEN_KEYWORD;
                                        token.text_len = character_iteration + 1;
                                        l->cursor += character_iteration + 1;
                                        return token;
                                }
                        }

                }
                character_iteration += 1;
        }


        //IDENTIFIERS
        if (is_symbol_start(l->content[l->cursor])) {
                token.kind = TOKEN_IDENTIFIER;
                while (l->cursor < l->content_length && is_symbol(l->content[l->cursor])) {
                        l->cursor += 1;
                        token.text_len += 1;
                }
                return token;
        }

        if (is_seperator(l->content[l->cursor])) {
                token.text_len = 1;
                l->cursor += 1;
                token.kind = TOKEN_DELIMITER;
                return token;
        }

        if (is_string_start_end(l->content[l->cursor])) {
                token.kind = TOKEN_STRING;
                l->cursor += 1;
                token.text_len += 1;
                while (!is_string_start_end(l->content[l->cursor]) && l->cursor < l->content_length) {
                        if (l->content[l->cursor] == '\n') {
                                fprintf(stderr, "Unclosed string at line %zu, col %zu\n", token.location.row, token.location.col);
                                exit(69);
                        }
                        l->cursor++;
                        token.text_len++;
                }
                l->cursor += 1;
                token.text_len += 1;
                return token;
        }

        if (is_operator(l->content[l->cursor])) {
                token.text_len = 1;
                l->cursor += 1;
                token.kind = TOKEN_OPERATOR;
                return token;
        }

        if (is_number(l->content[l->cursor])) {
                token.kind = TOKEN_NUMBER;
                while (is_number(l->content[l->cursor])) {
                        token.text_len += 1;
                        l->cursor += 1;
                }
                return token;
        }

        if (is_comment_start(l->content[l->cursor])) {
                token.kind = TOKEN_COMMENT;
                l->cursor += 1;
                token.text_len += 1;
                while (!is_comment_end(l->content[l->cursor])) {
                        if (is_line_end(l->content[l->cursor])) {
                                break;
                        }
                        l->cursor += 1;
                        token.text_len += 1;
                }
                if (is_comment_end(l->content[l->cursor])) {
                        l->cursor += 1;
                        token.text_len += 1;
                }
                return token;
        }

        //if (is_string_start_end(c)) {
        //        while (!is_string_start_end(l->content[l->cursor]) && l->cursor < l->content_length) {
        //                l->cursor++;
        //                token.text_len++;
        //        }
        //        if (!is_string_start_end(token.text[token.text_len])) {
        //                fprintf(stderr, "Unclosed string starting at line %llu, col %llu", token.location.row, token.location.col);
        //                exit(69);
        //        }
        //        return token;
        //}

        fprintf(stderr, "UNIIMPLEMENTED, Char: %c, at position: %llu\n", l->content[l->cursor], l->cursor);
        exit(420);
}



char *token_kind_name(Token_Kind kind) {
        switch (kind) {
                case TOKEN_END: { return "token end"; }
                case TOKEN_IDENTIFIER: { return "identifier"; }
                case TOKEN_NUMBER: { return "number"; }
                case TOKEN_STRING: { return "string"; }
                case TOKEN_KEYWORD: { return "keyword"; }
                case TOKEN_OPERATOR: { return "operator"; }
                case TOKEN_DELIMITER: { return "delimiter"; }
                case TOKEN_COMMENT: return "comment";
        }
        return NULL;
}


Token *collect_tokens(Lexer *lexer, size_t *out_count) {
        size_t capacity = 64;
        size_t count = 0;
        Token *tokens = malloc(capacity * sizeof(Token));
        if (!tokens) {
                fprintf(stderr, "Failed to allocate memory\n");
                exit(5137);
        }

        while (1) {
                Token token = lexer_next(lexer);

                fprintf(stderr, "%.*s (%s)\n", (int) token.text_len, token.text, token_kind_name(token.kind));
                if (count >= capacity) {
                        capacity *= 2;
                        Token *more_tokens = realloc(tokens, capacity * sizeof(Token));
                        if (!more_tokens) {
                                fprintf(stderr, "Failed to realloc memory\n");
                                exit(1337);
                        }
                        tokens = more_tokens;
                }

                tokens[count++] = token;

                if (token.kind == TOKEN_END) {
                        break;
                }
        }

        *out_count = count;
        return tokens;
}