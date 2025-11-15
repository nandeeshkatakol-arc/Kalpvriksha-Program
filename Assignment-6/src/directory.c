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
        freeHead = node;
        freeTail = node;
    }
    else
    {
        freeTail->next = node;
        node->prev = freeTail;
        freeTail = node;
    }
    return;
}

void setupFileSystem()
{
    int i;
    for (i = 0; i < NUM_BLOCKS; ++i)
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
    {
        int k;
        for (k = 0; k < MAX_BLOCKS_PER_FILE; ++k)
        {
            root->blockNumbers[k] = -1;
        }
    }
    root->firstChild = NULL;
    root->next = root;
    root->prev = root;
    root->parent = NULL;
    cwd = root;
    printf("Virtual File System initialized. Root '/' created.\n");
    return;
}

void makeDirectory(const char *name)
{
    int ok = 1;
    if (!name)
    {
        ok = 0;
    }
    else
    {
        if (strlen(name) == 0)
        {
            ok = 0;
        }
    }
    if (ok)
    {
        if (findChild(name) != NULL)
        {
            printf("Name '%s' already exists.\n", name);
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
        node->isDirectory = 1;
        node->fileSize = 0;
        node->numBlocks = 0;
        {
            int i;
            for (i = 0; i < MAX_BLOCKS_PER_FILE; ++i)
            {
                node->blockNumbers[i] = -1;
            }
        }
        node->firstChild = NULL;
        node->parent = cwd;
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
        if (ok)
        {
            printf("Directory '%s' created successfully.\n", name);
        }
    }
    return;
}

void listFiles()
{
    int ok = 1;
    if (!cwd)
    {
        ok = 0;
    }
    if (ok)
    {
        if (!cwd->firstChild)
        {
            printf("(empty)\n");
            ok = 0;
        }
    }
    if (ok)
    {
        FileNode *iter = cwd->firstChild;
        if (iter)
        {
            do
            {
                if (iter->isDirectory)
                {
                    printf("%s/\n", iter->name);
                }
                else
                {
                    printf("%s\n", iter->name);
                }
                iter = iter->next;
            }
            while (iter != cwd->firstChild);
        }
    }
    return;
}

void changeDirectory(const char *name)
{
    int ok = 1;
    if (!cwd)
    {
        ok = 0;
    }
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
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
            ok = 0;
        }
    }
    if (ok)
    {
        if (!cwd->firstChild)
        {
            printf("Directory not found.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        FileNode *iter = cwd->firstChild;
        int found = 0;
        if (iter)
        {
            do
            {
                if (iter->isDirectory)
                {
                    if (strcmp(iter->name, name) == 0)
                    {
                        cwd = iter;
                        found = 1;
                        break;
                    }
                }
                iter = iter->next;
            }
            while (iter != cwd->firstChild);
        }
        if (!found)
        {
            printf("Directory not found.\n");
        }
    }
    return;
}

void showPath()
{
    int ok = 1;
    if (!cwd)
    {
        ok = 0;
    }
    if (ok)
    {
        FileNode *iter = cwd;
        char buffer[1024];
        buffer[0] = '\0';
        while (iter)
        {
            char tmp[1024];
            if (strcmp(iter->name, "/") == 0)
            {
                if (strlen(buffer) == 0)
                {
                    strncpy(buffer, "/", sizeof(buffer) - 1);
                    buffer[sizeof(buffer) - 1] = '\0';
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
                    snprintf(tmp, sizeof(tmp), "%s", iter->name);
                }
                else
                {
                    snprintf(tmp, sizeof(tmp), "%s/%s", iter->name, buffer);
                }
                strncpy(buffer, tmp, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
            }
            iter = iter->parent;
        }
        if (strlen(buffer) > 1)
        {
            if (buffer[strlen(buffer) - 1] == '/')
            {
                buffer[strlen(buffer) - 1] = '\0';
            }
        }
        printf("%s\n", buffer);
    }
    return;
}

void removeDirectory(const char *name)
{
    int ok = 1;
    FileNode *target = NULL;
    if (!cwd)
    {
        ok = 0;
    }
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
        target = findChild(name);
        if (!target)
        {
            printf("Directory not found.\n");
            ok = 0;
        }
    }
    if (ok)
    {
        if (!target->isDirectory)
        {
            printf("'%s' is a file, not a directory.\n", name);
            ok = 0;
        }
    }
    if (ok)
    {
        if (target->firstChild != NULL)
        {
            printf("Cannot remove '%s': directory not empty.\n", name);
            ok = 0;
        }
    }
    if (ok)
    {
        FileNode *parent = target->parent;
        if (!parent)
        {
            printf("Cannot remove root.\n");
            ok = 0;
        }
        else
        {
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
    }
    return;
}

FileNode *findChild(const char *name)
{
    FileNode *result = NULL;
    int ok = 1;
    if (!cwd)
    {
        ok = 0;
    }
    if (!name)
    {
        ok = 0;
    }
    if (ok)
    {
        if (!cwd->firstChild)
        {
            ok = 0;
        }
    }
    if (ok)
    {
        FileNode *iter = cwd->firstChild;
        if (iter)
        {
            do
            {
                if (strcmp(iter->name, name) == 0)
                {
                    result = iter;
                    break;
                }
                iter = iter->next;
            }
            while (iter != cwd->firstChild && result == NULL);
        }
    }
    return result;
}

int getFreeBlock()
{
    int index = -1;
    if (!freeHead)
    {
        return index;
    }
    FreeBlock *node = freeHead;
    index = node->number;
    if (freeHead == freeTail)
    {
        freeHead = NULL;
        freeTail = NULL;
    }
    else
    {
        freeHead = freeHead->next;
        if (freeHead)
        {
            freeHead->prev = NULL;
        }
    }
    free(node);
    usedBlocks++;
    return index;
}

void returnBlock(int number)
{
    int ok = 1;
    if (number < 0 || number >= NUM_BLOCKS)
    {
        ok = 0;
    }
    if (ok)
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
            freeHead = node;
            freeTail = node;
        }
        else
        {
            freeTail->next = node;
            node->prev = freeTail;
            freeTail = node;
        }
        usedBlocks--;
    }
    return;
}

void clearFileBlocks(FileNode *file)
{
    int ok = 1;
    if (!file)
    {
        ok = 0;
    }
    if (ok)
    {
        int i;
        for (i = 0; i < file->numBlocks; ++i)
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
    return;
}

static void freeFileNodesInternal(FileNode *node)
{
    int ok = 1;
    if (!node)
    {
        ok = 0;
    }
    if (ok)
    {
        if (node->firstChild)
        {
            FileNode *start = node->firstChild;
            FileNode *child = start;
            if (child)
            {
                do
                {
                    FileNode *next = child->next;
                    child->next = NULL;
                    child->prev = NULL;
                    freeFileNodesInternal(child);
                    child = next;
                }
                while (child != start);
            }
        }
        free(node);
    }
    return;
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
    freeHead = NULL;
    freeTail = NULL;
    usedBlocks = 0;
    return;
}

void cleanupFileSystem()
{
    int ok = 1;
    if (!root)
    {
        ok = 0;
    }
    if (ok)
    {
        freeFileNodesInternal(root);
        freeFreeBlocksInternal();
        root = NULL;
        cwd = NULL;
        printf("Memory released. Exiting.\n");
    }
    return;
}

void showDiskInfo()
{
    int total = NUM_BLOCKS;
    int used = usedBlocks;
    int freeCount = total - used;
    double percent = 0.0;
    if (total > 0)
    {
        percent = ((double)used / (double)total) * 100.0;
    }
    printf("Total Blocks: %d\n", total);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", percent);
    return;
}
