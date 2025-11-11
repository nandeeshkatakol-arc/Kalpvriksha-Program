#ifndef VFS_H
#define VFS_H

#include "file.h"

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *prev;
    struct FreeBlock *next;
} FreeBlock;

typedef struct vfs
{
    FileNode *root;
    FileNode *cwd;
    int totalBlocks;
    int blockSize;
    int usedBlocks;
    unsigned char *disk;
    FreeBlock *freeHead;
    FreeBlock *freeTail;
} VFS;

VFS *createVfs(int totalBlocks, int blockSize);
void destroyVfs(VFS *vfs);
void showDiskInfo(VFS *vfs);

void makeDirectory(VFS *vfs, const char *name);
void changeDirectory(VFS *vfs, const char *path);
void createFile(VFS *vfs, const char *name);
void writeFileToVFS(VFS *vfs, const char *name, const char *data);
void readFileFromVFS(VFS *vfs, const char *name);
void deleteFileFromVFS(VFS *vfs, const char *name);
void listCurrentDirectory(VFS *vfs);
void removeDirectory(VFS *vfs, const char *name);

#endif
