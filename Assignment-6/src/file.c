#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vfs.h"

FileNode *createNode(const char *name, int isdirectory, FileNode *parent)
{
    FileNode *newnode = (FileNode*)safeMalloc(sizeof(FileNode));
    
    strncpy(newnode->name, name, MAX_NAME_LENGTH - 1);
    newnode->name[MAX_NAME_LENGTH - 1] = '\0';

    newnode->isDirectory = isdirectory;
    newnode->parent = parent;
    newnode->child = NULL;
    newnode->next = NULL;
    newnode->numBlocks = 0;
    newnode->size = 0;
    newnode->blockPointers = NULL;

    return newnode;
}

void addChild(FileNode *parent, FileNode *child)
{
    if (!parent->isDirectory) {
        printf("Error: Cannot add child to a file node.\n");
        return;
    }
    if (parent->child == NULL) {
        parent->child = child;
    } else {
        FileNode *temp = parent->child;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = child;
    }
    child->next = NULL;
    child->parent = parent;
}

FileNode *findChild(FileNode *parent, const char *name)
{
    if (!parent->isDirectory) {
        printf("Error: %s is not a directory.\n", parent->name);
        return NULL;
    }
    FileNode *temp = parent->child;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void listDirectory(FileNode *dir)
{
    if(!dir->isDirectory)
    {
        printf("Error: %s is not a Directory.\n",dir->name);
        return;
    }

    FileNode *temp = dir->child;

    if(!temp)
    {
        printf("Directory i empty.\n");
        return;
    }

    while(temp != NULL)
    {
        if(temp->isDirectory)
        {
            printf("%s/ \n",temp->name);
        }
        else
        {
            printf("%s \n",temp->name);
        }
        temp=temp->next;
    }

}

void deleteNode(FileNode *node)
{
    if (node == NULL)
        return;

    if (node->parent != NULL)
    {
        FileNode *parent = node->parent;
        FileNode *temp = parent->child;

        if (temp == node)
        {
            parent->child = node->next;
        }
        else
        {
            while (temp != NULL && temp->next != node)
            {
                temp = temp->next;
            }

            if (temp != NULL)
            {
                temp->next = node->next;
            }
        }
    }

    if (node->isDirectory)
    {
        FileNode *child = node->child;
        while (child != NULL)
        {
            FileNode *nextChild = child->next;
            deleteNode(child);
            child = nextChild;
        }
    }

    if (!node->isDirectory && node->blockPointers)
    {
        free(node->blockPointers);
        node->blockPointers = NULL;
    }

    node->parent = NULL;
    node->child = NULL;
    node->next = NULL;

    free(node);
}


void printPath(FileNode *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->parent == NULL)
    {
        printf("/");
        return;
    }

    printPath(node->parent);
    if (strcmp(node->name, "/") != 0)
        printf("%s/", node->name);
}