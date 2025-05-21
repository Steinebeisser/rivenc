//
// Created by Geisthardt on 19.05.2025.
//

#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>

typedef struct {
        const char *content;
        size_t content_length;
} File_Information;

File_Information *read_file(char *filename);

#endif //UTILS_H
