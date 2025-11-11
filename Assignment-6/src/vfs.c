#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"
#include "file.h"
#include "utils.h"

static void initFreeList(VFS *vfs);
static int popFreeHead(VFS *vfs);
static void appendFreeTail(VFS *vfs, int index);
static int *allocateBlocks(VFS *vfs, int count);
static void freeBlocksArray(VFS *vfs, int *arr, int count);
static void unlinkFromParent(FileNode *node);
static void writeToBlock(VFS *vfs, int blockIndex, const unsigned char *src, int bytesToWrite);
static void readFromBlock(VFS *vfs, int blockIndex, unsigned char *dest, int bytesToRead);

VFS *createVfs(int totalBlocks, int blockSize)
{
    if(totalBlocks < 1 || totalBlocks > 5000)
    {
        printf("createVFS: Total blocks must be between 1 and 5000.\n");
        return NULL;
    }

    if(blockSize <= 0)
    {
        printf("createVfs: invalid blockSize\n");
        return NULL;
    }
    VFS *vfs = (VFS *)safeMalloc(sizeof(VFS));
    vfs->totalBlocks = totalBlocks;
    vfs->blockSize = blockSize;
    vfs->usedBlocks = 0;
    vfs->freeHead = vfs->freeTail = NULL;
    vfs->disk = (unsigned char *)safeMalloc((size_t)totalBlocks * (size_t)blockSize);
    memset(vfs->disk, 0, (size_t)totalBlocks * (size_t)blockSize);

    initFreeList(vfs);

    vfs->root = createNode("/", 1, NULL);
    FileNode *home = createNode("home", 1, vfs->root);
    addChild(vfs->root, home);
    vfs->cwd = home;
    printf("VFS is created . Default directory = /home\n");
    return vfs;

}

static void recurseFree(VFS *vfs, FileNode *node)
{
    if(!node)
    {
        return;
    }
    if(node->isDirectory)
    {
        FileNode *child = node->child;
        while(child)
        {
            FileNode *nextChild = child->next;
            recurseFree(vfs,child);
            child = nextChild;
        }
    }
    else
    {
        if(node->numBlocks > 0 && node->blockPointers)
        {
            freeBlocksArray(vfs, node->blockPointers, node->numBlocks);
            node->blockPointers = NULL;
            node->numBlocks = 0;
            node->size = 0;
        }
    }
    unlinkFromParent(node);
    free(node);
}
void destroyVfs(VFS *vfs)
{
    if(!vfs)
    {
        return;
    }

    recurseFree(vfs,vfs->root);
    vfs->root = vfs->cwd = NULL;
    FreeBlock *curr = vfs->freeHead;
    while(curr)
    {
        FreeBlock *next = curr->next;
        free(curr);
        curr = next;
    }
    if(vfs->disk)
    {
        free(vfs->disk);
        vfs->disk = NULL;
    }

    free(vfs); 
}

static void initFreeList(VFS *vfs)
{
    for(int i =0; i < vfs->totalBlocks; ++i)
    {
        FreeBlock *node = (FreeBlock *)safeMalloc(sizeof(FreeBlock));
        node->index = i;
        node->prev = vfs->freeTail;
        node->next = NULL;
        
        if(vfs->freeTail)
        {
            vfs->freeTail->next = node;
        }
        vfs->freeTail = node;
        if(!vfs->freeHead)
        {
            vfs->freeHead = node;
        }
    }
    vfs->usedBlocks = 0;
}

static int popFreeHead(VFS *vfs)
{
    if(!vfs->freeHead)
    {
        return -1;
    }
    FreeBlock *node = vfs->freeHead;
    int idx = node->index;
    vfs->freeHead = node->next;
    if(vfs->freeHead)
    {
        vfs->freeHead->prev = NULL;
    }
    else
    {
        vfs->freeTail = NULL;
    }
    free(node);
    vfs->usedBlocks++;
    return idx;
}

static void appendFreeTail(VFS *vfs, int index)
{
    FreeBlock *node = (FreeBlock *)safeMalloc(sizeof(FreeBlock));
    node->index = index;
    node->next = NULL;
    node->prev = vfs->freeTail;

    if (vfs->freeTail)
        vfs->freeTail->next = node;

    vfs->freeTail = node;
    if (!vfs->freeHead)
        vfs->freeHead = node;

    vfs->usedBlocks--;
}

static int *allocateBlocks(VFS *vfs, int count)
{
    if(count < 1)
    {
        return NULL;
    }
    int available = vfs->totalBlocks - vfs->usedBlocks;
    if(available < count)
    {
        return NULL;
    }

    int *arr = (int *)safeMalloc(sizeof(int) *count);
    for(int i = 0; i <count ; i++)
    {
        int idx = popFreeHead(vfs);
        if(idx < 0)
        {
            for (int j = 0; j < i; j++)
            {
                appendFreeTail(vfs, arr[j]);
            }
            free(arr);
            return NULL;
        }
        arr[i] = idx;
    }
    return arr;
}

static void freeBlocksArray(VFS *vfs, int *arr, int count)
{
    if (!arr)
        return;

    for (int i = 0; i < count; i++)
        appendFreeTail(vfs, arr[i]);

    free(arr);
}

static void unlinkFromParent(FileNode *node)
{
    if (!node || !node->parent)
        return;

    FileNode *parent = node->parent;
    FileNode *curr = parent->child;

    if (curr == node)
    {
        parent->child = node->next;
    }
    else
    {
        while (curr && curr->next != node)
            curr = curr->next;
        if (curr)
            curr->next = node->next;
    }

    node->parent = NULL;
    node->next = NULL;
}

static void writeToBlock(VFS *vfs, int blockIndex, const unsigned char *src, int bytesToWrite)
{
    unsigned char *blockAddr = vfs->disk + (size_t)blockIndex * (size_t)vfs->blockSize;
    memcpy(blockAddr, src, (size_t)bytesToWrite);

    if (bytesToWrite < vfs->blockSize)
        memset(blockAddr + bytesToWrite, 0, (size_t)(vfs->blockSize - bytesToWrite));
}

static void readFromBlock(VFS *vfs, int blockIndex, unsigned char *dest, int bytesToRead)
{
    unsigned char *blockAddr = vfs->disk + (size_t)blockIndex * (size_t)vfs->blockSize;
    memcpy(dest, blockAddr, (size_t)bytesToRead);
}

void makeDirectory(VFS *vfs, const char *name)
{
    if (!vfs || !name)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    if (findChild(vfs->cwd, name))
    {
        printf("Error: Directory '%s' already exists.\n", name);
        return;
    }

    FileNode *newDir = createNode(name, 1, vfs->cwd);
    addChild(vfs->cwd, newDir);
    printf("Directory '%s' created successfully.\n", name);
}

void createFile(VFS *vfs, const char *name)
{
    if (!vfs || !name)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    if (findChild(vfs->cwd, name))
    {
        printf("Error: File '%s' already exists.\n", name);
        return;
    }

    FileNode *newFile = createNode(name, 0, vfs->cwd);
    addChild(vfs->cwd, newFile);
    printf("File '%s' created successfully.\n", name);
}

void writeFileToVFS(VFS *vfs, const char *name, const char *data)
{
    if (!vfs || !name || !data)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    FileNode *file = findChild(vfs->cwd, name);
    if (!file || file->isDirectory)
    {
        printf("Error: File '%s' not found.\n", name);
        return;
    }

    int dataSize = strlen(data);
    file->size = dataSize;

    int blocksNeeded = (dataSize + vfs->blockSize - 1) / vfs->blockSize;
    int *allocated = allocateBlocks(vfs, blocksNeeded);

    if (!allocated)
    {
        printf("Error: Not enough free blocks.\n");
        return;
    }

    file->blockPointers = allocated;
    file->numBlocks = blocksNeeded;

    for (int i = 0; i < blocksNeeded; i++)
    {
        int bytesToWrite = (i == blocksNeeded - 1)
                               ? (dataSize - i * vfs->blockSize)
                               : vfs->blockSize;

        writeToBlock(vfs, allocated[i],
                     (const unsigned char *)(data + i * vfs->blockSize),
                     bytesToWrite);
    }

    printf("Data written successfully to '%s' (size=%d bytes).\n", name, dataSize);
}

void readFileFromVFS(VFS *vfs, const char *name)
{
    if (!vfs || !name)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    FileNode *file = findChild(vfs->cwd, name);
    if (!file || file->isDirectory)
    {
        printf("Error: File '%s' not found.\n", name);
        return;
    }

    if (file->numBlocks == 0 || !file->blockPointers)
    {
        printf("File '%s' is empty.\n", name);
        return;
    }

    unsigned char *buffer = (unsigned char *)malloc(file->size + 1);
    int bytesRead = 0;

    for (int i = 0; i < file->numBlocks; i++)
    {
        int bytesToRead = (i == file->numBlocks - 1)
                              ? (file->size - i * vfs->blockSize)
                              : vfs->blockSize;

        readFromBlock(vfs, file->blockPointers[i],
                      buffer + bytesRead, bytesToRead);
        bytesRead += bytesToRead;
    }

    buffer[file->size] = '\0';
    printf("%s\n", buffer);
    free(buffer);
}

void deleteFileFromVFS(VFS *vfs, const char *name)
{
    if (!vfs || !name)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    FileNode *file = findChild(vfs->cwd, name);
    if (!file || file->isDirectory)
    {
        printf("Error: File '%s' not found.\n", name);
        return;
    }

    if (file->numBlocks > 0 && file->blockPointers)
    {
        freeBlocksArray(vfs, file->blockPointers, file->numBlocks);
        file->blockPointers = NULL;
        file->numBlocks = 0;
        file->size = 0;
    }

    deleteNode(file);
    printf("File '%s' deleted successfully.\n", name);
}

void removeDirectory(VFS *vfs, const char *name)
{
    if (!vfs || !name)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    FileNode *dir = findChild(vfs->cwd, name);
    if (!dir || !dir->isDirectory)
    {
        printf("Error: Directory '%s' not found.\n", name);
        return;
    }

    if (dir->child != NULL)
    {
        printf("Error: Directory '%s' is not empty.\n", name);
        return;
    }

    deleteNode(dir);
    printf("Directory '%s' removed successfully.\n", name);
}

void listCurrentDirectory(VFS *vfs)
{
    if (!vfs)
    {
        printf("Error: Invalid VFS reference.\n");
        return;
    }

    listDirectory(vfs->cwd);
}

void changeDirectory(VFS *vfs, const char *path)
{
    if (!vfs || !path)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    if (strcmp(path, "..") == 0)
    {
        if (vfs->cwd->parent)
        {
            vfs->cwd = vfs->cwd->parent;
            printf("Moved to parent directory.\n");
        }
        else
        {
            printf("Already in root directory.\n");
        }
        return;
    }

    FileNode *dir = findChild(vfs->cwd, path);
    if (dir && dir->isDirectory)
    {
        vfs->cwd = dir;
        printf("Moved to /%s\n", dir->name);
    }
    else
    {
        printf("Error: Directory '%s' not found.\n", path);
    }
}

void showDiskInfo(VFS *vfs)
{
    if (!vfs)
    {
        printf("Error: Invalid VFS reference.\n");
        return;
    }

    printf("Total Blocks: %d\n", vfs->totalBlocks);
    printf("Used Blocks: %d\n", vfs->usedBlocks);
    printf("Free Blocks: %d\n", vfs->totalBlocks - vfs->usedBlocks);
    printf("Disk Usage: %.2f%%\n",
           (vfs->totalBlocks > 0)
               ? ((double)vfs->usedBlocks / vfs->totalBlocks) * 100.0
               : 0.0);
}
