#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void *safeMalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        fprintf(stderr, "Memory allocation failed! Requested: %lu bytes\n", (unsigned long)size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void printLine()
{
    printf("------------------------------------------------------------\n");
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; 
}

void formatSize(size_t bytes, char *buffer, size_t bufSize)
{
    if (bytes < 1024)
        snprintf(buffer, bufSize, "%lu B", bytes);
    else if (bytes < 1024 * 1024)
        snprintf(buffer, bufSize, "%.2f KB", (double)bytes / 1024.0);
    else
        snprintf(buffer, bufSize, "%.2f MB", (double)bytes / (1024.0 * 1024.0));
}