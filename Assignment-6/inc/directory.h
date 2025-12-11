#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LEN 50
#define MAX_BLOCKS_PER_FILE 10

typedef struct FileNode
{
    char name[MAX_NAME_LEN + 1];
    int isDirectory;
    int fileSize;
    int numBlocks;
    int blockNumbers[MAX_BLOCKS_PER_FILE];

    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *parent;
    struct FileNode *firstChild;
} FileNode;

typedef struct FreeBlock
{
    int number;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

extern unsigned char disk[NUM_BLOCKS][BLOCK_SIZE];
extern FreeBlock *freeHead;
extern FreeBlock *freeTail;
extern FileNode *root;
extern FileNode *cwd;
extern int usedBlocks;

void setupFileSystem();
void makeDirectory(const char *name);
void listFiles();
void changeDirectory(const char *name);
void showPath();
void removeDirectory(const char *name);
void showDiskInfo();

FileNode *findChild(FileNode *directory, const char *name);
int getFreeBlock();
void returnBlock(int number);
void clearFileBlocks(FileNode *file);
void cleanupFileSystem();

#endif
