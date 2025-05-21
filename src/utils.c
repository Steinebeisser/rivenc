//
// Created by Geisthardt on 19.05.2025.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

File_Information *read_file(char *filename) {
        FILE *fd = fopen(filename, "rb");
        if (!fd) {
                perror("fopen");
                return NULL;
        }

        File_Information *fi = malloc(sizeof(File_Information));
        if (!fi) {
                perror("malloc");
                fclose(fd);
                return NULL;
        }

        if (fseek(fd, 0, SEEK_END) != 0) {
                perror("fseek");
                fclose(fd);
                return NULL;
        }
        long size = ftell(fd);
        if (size < 0) {
                perror("ftell");
                fclose(fd);
                return NULL;
        }
        rewind(fd);
        fi->content_length = size;

        char *buffer = malloc((size_t)size);
        if (!buffer) {
                perror("malloc");
                fclose(fd);
                free(fi);
                return NULL;
        }

        size_t read_bytes = fread(buffer, 1, (size_t)size, fd);
        if (read_bytes != (size_t)size) {
                fprintf(stderr, "read_file: expected %ld bytes but read %zu\n", size, read_bytes);
                free(fi);
                free(buffer);
                fclose(fd);
                return NULL;
        }

        fclose(fd);

        fi->content = buffer;

        return fi;
}
