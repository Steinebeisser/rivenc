//
// Created by Geisthardt on 28.05.2025.
//

#include "asm.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

char *generate_asm(Ast_Node *program_node) {//returns filepath not asm code
        char *filename = "out.asm";
        FILE *fd = fopen("out.asm", "w");
        if (!fd) {
                fprintf(stderr, "Failed to open out.asm\n");
                exit(-1);
        }

        fprintf(fd, "format ELF64 executable\n\n"); //specify platform, currently only linux
        fprintf(fd, "_start:\n");

        generate_node(fd, program_node);

        fprintf(fd, "    mov rdi, rax\n");
        fprintf(fd, "    mov rax, 60\n");
        fprintf(fd, "    syscall\n");

        fclose(fd);

        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
                fprintf(stderr, "Failed to get filepath\n");
                exit(-1);
        }

        size_t path_len = strlen(cwd) + 1 + strlen(filename) + 1;
        char *full_path = malloc(path_len);
        if (!full_path) {
                fprintf(stderr, "failed to alloc memory\n");
        }

        snprintf(full_path, path_len, "%s/%s", cwd, filename);
        return full_path;
}

bool compile_asm_with_fasm(char *filepath) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "fasm \"%s\"", filepath);
        system(cmd);
        return true;
}

void generate_node(FILE *fd, Ast_Node *node) {
        switch (node->type) {
                case NODE_PROGRAM: {
                        for (size_t i = 0; i < node->data.program.count; ++i) {
                                generate_node(fd, node->data.program.children[i]);
                        }
                } break;
                case NODE_VARIABLE_DECLARATION: {
                        if (node->data.variable_declaration.initializer) {
                                generate_node(fd, node->data.variable_declaration.initializer);
                        } else {
                                fprintf(stderr, "doesnt support declaration without initialization\n");
                                exit(20);
                        }
                } break;
                case NODE_BINARY_EXPRESSION: {
                        generate_node(fd, node->data.binary_expression.left);
                        fprintf(fd, "    push rax\n");

                        generate_node(fd, node->data.binary_expression.right);
                        fprintf(fd, "    mov rbx, rax\n");

                        fprintf(fd, "    pop rax\n");


                        if (strcmp(node->data.binary_expression.operator, "+") == 0) {
                                fprintf(fd, "    add rax, rbx\n");
                        } else if (strcmp(node->data.binary_expression.operator, "-") == 0) {
                                fprintf(fd, "    sub rax, rbx\n");
                        } else if (strcmp(node->data.binary_expression.operator, "*") == 0) {
                                fprintf(fd, "    imul rax, rbx\n");
                        } else if (strcmp(node->data.binary_expression.operator, "/") == 0) {
                                fprintf(fd, "    cqo\n");
                                fprintf(fd, "    idiv rbx\n");
                        } else {
                                fprintf(stderr, "unsupported binary expression %s\n", node->data.binary_expression.operator);
                                exit(21);
                        }
                        fprintf(fd, "\n");
                } break;
                case NODE_NUMBER: {
                        fprintf(fd, "    mov rax, %lu\n", node->data.number.value);
                } break;
                case NODE_IDENTIFIER: {

                } break;
        }
}
