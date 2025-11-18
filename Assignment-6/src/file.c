#include "file.h"

void createFile(const char *name)
{
    int isValid = 1;
    if (!name)
    {
        isValid = 0;
    }
    if (isValid)
    {
        if (findChild(cwd,name) != NULL)
        {
            printf("Name '%s' already exists in current directory.\n", name);
            isValid = 0;
        }
    }
    if (isValid)
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
}

void writeFile(const char *name, const char *data)
{
    if (!name || !data)
    {
        printf("Invalid file name or data.\n");
        return;
    }

    FileNode *fileNode = findChild(cwd, name);
    if (!fileNode)
    {
        printf("File not found.\n");
        return;
    }

    if (fileNode->isDirectory)
    {
        printf("'%s' is a directory, not a file.\n", name);
        return;
    }

    int dataSize = (int)strlen(data);
    int requiredBlocks = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (requiredBlocks > MAX_BLOCKS_PER_FILE)
    {
        printf("File too large. Max %d blocks allowed.\n", MAX_BLOCKS_PER_FILE);
        return;
    }

    int freeBlocksAvailable = NUM_BLOCKS - usedBlocks;
    if (freeBlocksAvailable < requiredBlocks)
    {
        printf("Not enough space on disk.\n");
        return;
    }

    if (fileNode->numBlocks > 0)
        clearFileBlocks(fileNode);

    int allocatedBlocks = 0;
    const unsigned char *dataPtr = (const unsigned char *)data;
    int bytesRemaining = dataSize;

    for (int iterator = 0; iterator < requiredBlocks; ++iterator)
    {
        int blockIndex = getFreeBlock();
        if (blockIndex == -1)
        {
            for (int restoreIndex = 0; restoreIndex < allocatedBlocks; ++restoreIndex)
            {
                returnBlock(fileNode->blockNumbers[restoreIndex]);
                fileNode->blockNumbers[restoreIndex] = -1;
            }

            fileNode->numBlocks = 0;
            fileNode->fileSize = 0;

            printf("Write failed. Not enough free blocks.\n");
            return;
        }

        int bytesToWrite = (bytesRemaining > BLOCK_SIZE) ? BLOCK_SIZE : bytesRemaining;

        memcpy(disk[blockIndex], dataPtr, bytesToWrite);

        if (bytesToWrite < BLOCK_SIZE)
            memset(disk[blockIndex] + bytesToWrite, 0, BLOCK_SIZE - bytesToWrite);

        fileNode->blockNumbers[iterator] = blockIndex;

        dataPtr += bytesToWrite;
        bytesRemaining -= bytesToWrite;
        allocatedBlocks++;
    }

    fileNode->numBlocks = allocatedBlocks;
    fileNode->fileSize = dataSize;

    printf("Data written successfully (size=%d bytes).\n", dataSize);
}

void readFile(const char *name)
{
    int isValid = 1;
    FileNode *file = NULL;
    if (!name)
    {
        isValid = 0;
    }
    if (isValid)
    {
        file = findChild(cwd,name);
        if (!file)
        {
            printf("File not found.\n");
            isValid = 0;
        }
    }
    if (isValid)
    {
        if (file->isDirectory)
        {
            printf("'%s' is a directory, not a file.\n", name);
            isValid = 0;
        }
    }
    if (isValid)
    {
        if (file->numBlocks == 0)
        {
            printf("File is empty.\n");
            isValid = 0;
        }
    }
    if (isValid)
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
}

void deleteFile(const char *name)
{
    int isValid = 1;
    FileNode *fileToDelete = NULL;
    if (!name)
    {
        isValid = 0;
    }
    if (isValid)
    {
        fileToDelete = findChild(cwd,name);
        if (!fileToDelete)
        {
            printf("File not found.\n");
            isValid = 0;
        }
    }
    if (isValid)
    {
        if (fileToDelete->isDirectory)
        {
            printf("'%s' is a directory. Use rmdir instead.\n", name);
            isValid = 0;
        }
    }
    if (isValid)
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
}
