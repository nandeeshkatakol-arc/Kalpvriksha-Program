#include "file.h"

void createFile(const char *name)
{
    if (findChild(name) != NULL)
    {
        printf("Name '%s' already exists in current directory.\n", name);
        return;
    }

    FileNode *node = (FileNode *)calloc(1, sizeof(FileNode));
    if (!node)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    strncpy(node->name, name, MAX_NAME_LEN);
    node->name[MAX_NAME_LEN] = '\0';
    node->isDirectory = 0;
    node->fileSize = 0;
    node->numBlocks = 0;
    for (int i = 0; i < MAX_BLOCKS_PER_FILE; ++i)
    {
        node->blockNumbers[i] = -1;
    }
    node->parent = cwd;
    node->firstChild = NULL;
    node->next = node->prev = NULL;

    if (!cwd->firstChild)
    {
        cwd->firstChild = node;
        node->next = node->prev = node;
    }
    else
    {
        FileNode *first = cwd->firstChild;
        FileNode *last = first->prev;
        last->next = node;
        node->prev = last;
        node->next = first;
        first->prev = node;
    }

    printf("File '%s' created successfully.\n", name);
}

void writeFile(const char *name, const char *data)
{
    FileNode *file = findChild(name);
    if (!file)
    {
        printf("File not found.\n");
        return;
    }

    if (file->isDirectory)
    {
        printf("'%s' is a directory, not a file.\n", name);
        return;
    }

    if (!data)
    {
        printf("No data provided.\n");
        return;
    }

    int dataSize = (int)strlen(data);
    int needed = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (needed > MAX_BLOCKS_PER_FILE)
    {
        printf("File too large. Max %d blocks allowed.\n", MAX_BLOCKS_PER_FILE);
        return;
    }

    int freeAvail = NUM_BLOCKS - usedBlocks;
    if (freeAvail < needed)
    {
        printf("Not enough space on disk.\n");
        return;
    }

    if (file->numBlocks > 0)
    {
        clearFileBlocks(file);
    }

    int allocated = 0;
    const unsigned char *ptr = (const unsigned char *)data;
    int remaining = dataSize;

    for (int i = 0; i < needed; ++i)
    {
        int blk = getFreeBlock();
        if (blk == -1)
        {
            for (int j = 0; j < allocated; ++j)
            {
                returnBlock(file->blockNumbers[j]);
                file->blockNumbers[j] = -1;
            }
            file->numBlocks = 0;
            file->fileSize = 0;
            printf("Error: not enough free blocks (during write).\n");
            return;
        }

        int toWrite = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
        memcpy(disk[blk], ptr, toWrite);
        if (toWrite < BLOCK_SIZE)
        {
            memset(disk[blk] + toWrite, 0, BLOCK_SIZE - toWrite);
        }

        file->blockNumbers[i] = blk;
        ptr += toWrite;
        remaining -= toWrite;
        ++allocated;
    }

    file->numBlocks = allocated;
    file->fileSize = dataSize;
    printf("Data written successfully (size=%d bytes).\n", dataSize);
}

void readFile(const char *name)
{
    FileNode *file = findChild(name);
    if (!file)
    {
        printf("File not found.\n");
        return;
    }

    if (file->isDirectory)
    {
        printf("'%s' is a directory, not a file.\n", name);
        return;
    }

    if (file->numBlocks == 0)
    {
        printf("File is empty.\n");
        return;
    }

    int printed = 0;
    for (int i = 0; i < file->numBlocks; ++i)
    {
        int blk = file->blockNumbers[i];
        int remain = file->fileSize - printed;
        int toPrint = (remain > BLOCK_SIZE) ? BLOCK_SIZE : remain;
        fwrite(disk[blk], 1, toPrint, stdout);
        printed += toPrint;
    }
    printf("\n");
}

void deleteFile(const char *name)
{
    FileNode *file = findChild(name);
    if (!file)
    {
        printf("File not found.\n");
        return;
    }

    if (file->isDirectory)
    {
        printf("'%s' is a directory. Use rmdir instead.\n", name);
        return;
    }

    for (int i = 0; i < file->numBlocks; ++i)
    {
        if (file->blockNumbers[i] >= 0)
        {
            returnBlock(file->blockNumbers[i]);
            file->blockNumbers[i] = -1;
        }
    }
    file->numBlocks = 0;
    file->fileSize = 0;

    FileNode *parent = file->parent;
    if (file->next == file && parent->firstChild == file)
    {
        parent->firstChild = NULL;
    }
    else
    {
        file->prev->next = file->next;
        file->next->prev = file->prev;
        if (parent->firstChild == file)
        {
            parent->firstChild = file->next;
        }
    }

    free(file);
    printf("File '%s' deleted successfully.\n", name);
}
