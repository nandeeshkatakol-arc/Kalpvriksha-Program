#include "file.h"

void createFile(const char *name)
{
    int ok = 1;
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
        if (findChild(name) != NULL)
        {
            printf("Name '%s' already exists in current directory.\n", name);
            ok = 0;
        }
    }
    if (ok)
    {
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
        {
            int i;
            for (i = 0; i < MAX_BLOCKS_PER_FILE; ++i)
            {
                node->blockNumbers[i] = -1;
            }
        }
        node->parent = cwd;
        node->firstChild = NULL;
        node->next = NULL;
        node->prev = NULL;
        if (!cwd->firstChild)
        {
            cwd->firstChild = node;
            node->next = node;
            node->prev = node;
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
    return;
}

void writeFile(const char *name, const char *data)
{
    int ok = 1;
    FileNode *file = NULL;
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
        file = findChild(name);
        if (!file)
        {
            printf("File not found.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        if (file->isDirectory)
        {
            printf("'%s' is a directory, not a file.\n", name);
            ok = 0;
        }
    }
    if (ok)
    {
        if (!data)
        {
            printf("No data provided.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        int dataSize = (int)strlen(data);
        int needed = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (needed > MAX_BLOCKS_PER_FILE)
        {
            printf("File too large. Max %d blocks allowed.\n", MAX_BLOCKS_PER_FILE);
            ok = 0;
        }
        if (ok)
        {
            int freeAvail = NUM_BLOCKS - usedBlocks;
            if (freeAvail < needed)
            {
                printf("Not enough space on disk.\n");
                ok = 0;
            }
        }
        if (ok)
        {
            if (file->numBlocks > 0)
            {
                clearFileBlocks(file);
            }
            int allocated = 0;
            const unsigned char *ptr = (const unsigned char *)data;
            int remaining = dataSize;
            {
                int i;
                for (i = 0; i < needed; ++i)
                {
                    int blk = getFreeBlock();
                    if (blk == -1)
                    {
                        int j;
                        for (j = 0; j < allocated; ++j)
                        {
                            returnBlock(file->blockNumbers[j]);
                            file->blockNumbers[j] = -1;
                        }
                        file->numBlocks = 0;
                        file->fileSize = 0;
                        printf("Error: not enough free blocks (during write).\n");
                        ok = 0;
                        break;
                    }
                    {
                        int toWrite;
                        if (remaining > BLOCK_SIZE)
                        {
                            toWrite = BLOCK_SIZE;
                        }
                        else
                        {
                            toWrite = remaining;
                        }
                        memcpy(disk[blk], ptr, toWrite);
                        if (toWrite < BLOCK_SIZE)
                        {
                            memset(disk[blk] + toWrite, 0, BLOCK_SIZE - toWrite);
                        }
                        file->blockNumbers[i] = blk;
                        ptr += toWrite;
                        remaining -= toWrite;
                        allocated++;
                    }
                }
            }
            if (ok)
            {
                file->numBlocks = allocated;
                file->fileSize = dataSize;
                printf("Data written successfully (size=%d bytes).\n", dataSize);
            }
        }
    }
    return;
}

void readFile(const char *name)
{
    int ok = 1;
    FileNode *file = NULL;
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
        file = findChild(name);
        if (!file)
        {
            printf("File not found.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        if (file->isDirectory)
        {
            printf("'%s' is a directory, not a file.\n", name);
            ok = 0;
        }
    }
    if (ok)
    {
        if (file->numBlocks == 0)
        {
            printf("File is empty.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        int printed = 0;
        int i;
        for (i = 0; i < file->numBlocks; ++i)
        {
            int blk = file->blockNumbers[i];
            int remain = file->fileSize - printed;
            int toPrint;
            if (remain > BLOCK_SIZE)
            {
                toPrint = BLOCK_SIZE;
            }
            else
            {
                toPrint = remain;
            }
            fwrite(disk[blk], 1, toPrint, stdout);
            printed += toPrint;
        }
        printf("\n");
    }
    return;
}

void deleteFile(const char *name)
{
    int ok = 1;
    FileNode *fileToDelete = NULL;
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
        fileToDelete = findChild(name);
        if (!fileToDelete)
        {
            printf("File not found.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        if (fileToDelete->isDirectory)
        {
            printf("'%s' is a directory. Use rmdir instead.\n", name);
            ok = 0;
        }
    }
    if (ok)
    {
        int i;
        for (i = 0; i < fileToDelete->numBlocks; ++i)
        {
            if (fileToDelete->blockNumbers[i] >= 0)
            {
                returnBlock(fileToDelete->blockNumbers[i]);
                fileToDelete->blockNumbers[i] = -1;
            }
        }
        fileToDelete->numBlocks = 0;
        fileToDelete->fileSize = 0;
        {
            FileNode *parent = fileToDelete->parent;
            if (fileToDelete->next == fileToDelete && parent->firstChild == fileToDelete)
            {
                parent->firstChild = NULL;
            }
            else
            {
                fileToDelete->prev->next = fileToDelete->next;
                fileToDelete->next->prev = fileToDelete->prev;
                if (parent->firstChild == fileToDelete)
                {
                    parent->firstChild = fileToDelete->next;
                }
            }
        }
        free(fileToDelete);
        printf("File '%s' deleted successfully.\n", name);
    }
    return;
}
