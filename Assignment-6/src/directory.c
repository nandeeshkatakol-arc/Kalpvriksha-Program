#include "directory.h"

unsigned char disk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;
FileNode *root = NULL;
FileNode *cwd = NULL;
int usedBlocks = 0;

static void addFreeBlockInternal(int number)
{
    FreeBlock *node = (FreeBlock *)malloc(sizeof(FreeBlock));
    if (!node)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    node->number = number;
    node->next = NULL;
    node->prev = NULL;

    if (!freeHead)
    {
        freeHead = freeTail = node;
    }
    else
    {
        freeTail->next = node;
        node->prev = freeTail;
        freeTail = node;
    }
}

void setupFileSystem()
{
    for (int i = 0; i < NUM_BLOCKS; ++i)
    {
        addFreeBlockInternal(i);
    }

    root = (FileNode *)calloc(1, sizeof(FileNode));
    if (!root)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    strncpy(root->name, "/", MAX_NAME_LEN);
    root->name[MAX_NAME_LEN] = '\0';
    root->isDirectory = 1;
    root->fileSize = 0;
    root->numBlocks = 0;
    for (int i = 0; i < MAX_BLOCKS_PER_FILE; ++i)
    {
        root->blockNumbers[i] = -1;
    }
    root->firstChild = NULL;
    root->next = root->prev = root;
    root->parent = NULL;

    cwd = root;

    printf("Virtual File System initialized. Root '/' created.\n");
}

void makeDirectory(const char *name)
{
    if (!name || strlen(name) == 0)
    {
        printf("Invalid directory name.\n");
        return;
    }

    if (findChild(name) != NULL)
    {
        printf("Name '%s' already exists.\n", name);
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
    node->isDirectory = 1;
    node->fileSize = 0;
    node->numBlocks = 0;
    for (int i = 0; i < MAX_BLOCKS_PER_FILE; ++i)
    {
        node->blockNumbers[i] = -1;
    }
    node->firstChild = NULL;
    node->parent = cwd;
    node->next = node->prev = NULL;

    if (cwd->firstChild == NULL)
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

    printf("Directory '%s' created successfully.\n", name);
}

void listFiles()
{
    if (!cwd->firstChild)
    {
        printf("(empty)\n");
        return;
    }

    FileNode *iterator = cwd->firstChild;
    do
    {
        if (iterator->isDirectory)
        {
            printf("%s/\n", iterator->name);
        }
        else
        {
            printf("%s\n", iterator->name);
        }
        iterator = iterator->next;
    }
    while (iterator != cwd->firstChild);
}

void changeDirectory(const char *name)
{
    if (!name)
    {
        printf("Invalid path.\n");
        return;
    }

    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent)
        {
            cwd = cwd->parent;
        }
        else
        {
            printf("Already at root.\n");
        }
        return;
    }

    if (!cwd->firstChild)
    {
        printf("Directory not found.\n");
        return;
    }

    FileNode *iterator = cwd->firstChild;
    do
    {
        if (iterator->isDirectory && strcmp(iterator->name, name) == 0)
        {
            cwd = iterator;
            return;
        }
        iterator = iterator->next;
    }
    while (iterator != cwd->firstChild);

    printf("Directory not found.\n");
}

void showPath()
{
    FileNode *iterator = cwd;
    char buffer[1024] = "";
    while (iterator)
    {
        char tmp[1024];
        if (strcmp(iterator->name, "/") == 0)
        {
            if (strlen(buffer) == 0)
            {
                strcpy(buffer, "/");
            }
            else
            {
                snprintf(tmp, sizeof(tmp), "/%s", buffer);
                strncpy(buffer, tmp, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
            }
            break;
        }
        else
        {
            if (strlen(buffer) == 0)
            {
                snprintf(tmp, sizeof(tmp), "%s", iterator->name);
            }
            else
            {
                snprintf(tmp, sizeof(tmp), "%s/%s", iterator->name, buffer);
            }
            strncpy(buffer, tmp, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
        iterator = iterator->parent;
    }

    if (strlen(buffer) > 1 && buffer[strlen(buffer) - 1] == '/')
    {
        buffer[strlen(buffer) - 1] = '\0';
    }

    printf("%s\n", buffer);
}

void removeDirectory(const char *name)
{
    FileNode *target = findChild(name);
    if (!target)
    {
        printf("Directory not found.\n");
        return;
    }

    if (!target->isDirectory)
    {
        printf("'%s' is a file, not a directory.\n", name);
        return;
    }

    if (target->firstChild != NULL)
    {
        printf("Cannot remove '%s': directory not empty.\n", name);
        return;
    }

    FileNode *parent = target->parent;
    if (!parent)
    {
        printf("Cannot remove root.\n");
        return;
    }

    if (target->next == target && parent->firstChild == target)
    {
        parent->firstChild = NULL;
    }
    else
    {
        target->prev->next = target->next;
        target->next->prev = target->prev;
        if (parent->firstChild == target)
        {
            parent->firstChild = target->next;
        }
    }

    free(target);
    printf("Directory '%s' removed successfully.\n", name);
}

FileNode *findChild(const char *name)
{
    if (!cwd->firstChild)
    {
        return NULL;
    }

    FileNode *iterator = cwd->firstChild;
    do
    {
        if (strcmp(iterator->name, name) == 0)
        {
            return iterator;
        }
        iterator = iterator->next;
    }
    while (iterator != cwd->firstChild);

    return NULL;
}

int getFreeBlock()
{
    if (!freeHead)
    {
        return -1;
    }

    FreeBlock *node = freeHead;
    int num = node->number;

    if (freeHead == freeTail)
    {
        freeHead = freeTail = NULL;
    }
    else
    {
        freeHead = freeHead->next;
        freeHead->prev = NULL;
    }

    free(node);
    usedBlocks++;
    return num;
}

void returnBlock(int number)
{
    FreeBlock *node = (FreeBlock *)malloc(sizeof(FreeBlock));
    if (!node)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    node->number = number;
    node->next = NULL;
    node->prev = NULL;

    if (!freeTail)
    {
        freeHead = freeTail = node;
    }
    else
    {
        freeTail->next = node;
        node->prev = freeTail;
        freeTail = node;
    }

    usedBlocks--;
}

void clearFileBlocks(FileNode *file)
{
    if (!file)
    {
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
}

static void freeFileNodesInternal(FileNode *node)
{
    if (!node)
    {
        return;
    }

    if (node->firstChild)
    {
        FileNode *start = node->firstChild;
        FileNode *child = start;
        do
        {
            FileNode *next = child->next;
            child->next = child->prev = NULL;
            freeFileNodesInternal(child);
            child = next;
        }
        while (child != start);
    }

    free(node);
}

static void freeFreeBlocksInternal()
{
    FreeBlock *cur = freeHead;
    while (cur)
    {
        FreeBlock *next = cur->next;
        free(cur);
        cur = next;
    }
    freeHead = freeTail = NULL;
    usedBlocks = 0;
}

void cleanupFileSystem()
{
    freeFileNodesInternal(root);
    freeFreeBlocksInternal();
    root = cwd = NULL;
    printf("Memory released. Exiting.\n");
}

void showDiskInfo()
{
    int total = NUM_BLOCKS;
    int used = usedBlocks;
    int freeBlocks = total - used;
    double percent = (total > 0) ? ((double)used / total) * 100.0 : 0.0;

    printf("Total Blocks: %d\n", total);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Disk Usage: %.2f%%\n", percent);
}
