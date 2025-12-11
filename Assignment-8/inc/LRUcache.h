#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    int key;
    char *data;
    struct Node *previous;
    struct Node *next;
} Node;

typedef struct LRUCache
{
    int capacity;
    int currentSize;
    Node *head;
    Node *tail;
    Node **hashMap;
    int hashSize;
} LRUCache;

LRUCache* createCache(const int capacity);
char* getValue(LRUCache *cacheObject, const int key);
void putValue(LRUCache *cacheObject, const int key, const char *value);
void freeCache(LRUCache *cacheObject);

#endif
