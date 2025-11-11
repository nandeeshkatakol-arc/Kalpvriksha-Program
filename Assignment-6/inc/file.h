#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_BLOCKS_PER_FILE 20

typedef struct FileNode
{
    char name[MAX_NAME_LENGTH];
    int isDirectory;                // 1 for directory, 0 for file
    struct FileNode *child;         // Points to first child (file or subdirectory)
    struct FileNode *next;          // Next node in same directory
    struct FileNode *parent;        // Points to parent directory

    // For files only
    int *blockPointers;             // indices of allocated blocks
    int numBlocks;                  // how many blocks allocated
    int size;                       // size of content in bytes
} FileNode;

// Function declarations
FileNode *createNode(const char *name, int isDirectory, FileNode *parent);
void addChild(FileNode *parent, FileNode *child);
void deleteNode(FileNode *node);
FileNode *findChild(FileNode *parent, const char *name);
void listDirectory(FileNode *dir);
void printPath(FileNode *node);

#endif
