#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lexer.h"
#include "src/parser.h"
#include "src/utils.h"
int main(int argc, char**argv) {
        if (argc < 2) {
                printf("Correct usage: riven <file.rvn>\n");
                return 1;
        }

        printf("Filename: %s\n", argv[1]);
        File_Information *fi = read_file(argv[1]);
        printf("File length: %llu\n", fi->content_length);
        //printf("SOURCE: %s\n", fi->content);

        char *test = "test test test 123\nmehr test leine 2";
        Lexer lexer = lexer_create(fi->content, fi->content_length);
        //Lexer lexer = lexer_create(test, strlen(test));
        //lexer_next(&lexer);


        //Token token = lexer_next(&lexer);
        //while (token.kind != TOKEN_END)  {
        //        fprintf(stderr, "%.*s (%s)\n", (int) token.text_len, token.text, token_kind_name(token.kind));
        //        token = lexer_next(&lexer);
        //}

        size_t token_count = 0;
        Token *tokens = collect_tokens(&lexer, &token_count);

        printf("Parsed %llu tokens\n", token_count);

        Parser parser = parser_create(tokens, token_count);

        Ast_Node program_node = parser_parse(&parser);

        print_ast(&program_node);

        printf("EOF");
        return 0;
}