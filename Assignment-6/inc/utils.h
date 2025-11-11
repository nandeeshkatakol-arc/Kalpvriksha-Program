#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>   
#include <stdio.h>
void *safeMalloc(size_t size);
void clearInputBuffer(void);
void printLine(void);
void formatSize(size_t bytes, char *buffer, size_t bufSize);
#endif
