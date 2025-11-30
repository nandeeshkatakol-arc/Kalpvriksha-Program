#include "LRUcache.h"

static int computeHashIndex(const int key, const int hashSize)
{
    int hashIndex = key % hashSize;
    if (hashIndex < 0) hashIndex = -hashIndex;
    return hashIndex;
}

static Node* createNode(const int key, const char *data)
{
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) return NULL;
    newNode->key = key;
    newNode->previous = NULL;
    newNode->next = NULL;
    newNode->data = NULL;
    if (data != NULL)
    {
        size_t length = strlen(data);
        newNode->data = (char*)malloc(length + 1);
        if (newNode->data == NULL)
        {
            free(newNode);
            return NULL;
        }
        strcpy(newNode->data, data);
    }
    return newNode;
}

LRUCache* createCache(const int capacity)
{
    if (capacity <= 0) return NULL;
    LRUCache *cacheObject = (LRUCache*)malloc(sizeof(LRUCache));
    if (cacheObject == NULL) return NULL;
    cacheObject->capacity = capacity;
    cacheObject->currentSize = 0;
    cacheObject->head = NULL;
    cacheObject->tail = NULL;
    cacheObject->hashSize = capacity * 2;
    cacheObject->hashMap = (Node**)calloc(cacheObject->hashSize, sizeof(Node*));
    if (cacheObject->hashMap == NULL)
    {
        free(cacheObject);
        return NULL;
    }
    return cacheObject;
}

static void insertAtHead(LRUCache *cacheObject, Node *newNode)
{
    if (cacheObject->head == NULL)
    {
        cacheObject->head = newNode;
        cacheObject->tail = newNode;
        newNode->previous = NULL;
        newNode->next = NULL;
        return;
    }
    newNode->next = cacheObject->head;
    newNode->previous = NULL;
    cacheObject->head->previous = newNode;
    cacheObject->head = newNode;
}

static void removeNodeFromList(LRUCache *cacheObject, Node *nodeToRemove)
{
    if (nodeToRemove == NULL) return;
    if (nodeToRemove == cacheObject->head)
    {
        cacheObject->head = nodeToRemove->next;
        if (cacheObject->head != NULL) cacheObject->head->previous = NULL;
    }
    else if (nodeToRemove == cacheObject->tail)
    {
        cacheObject->tail = nodeToRemove->previous;
        if (cacheObject->tail != NULL) cacheObject->tail->next = NULL;
    }
    else
    {
        if (nodeToRemove->previous != NULL) nodeToRemove->previous->next = nodeToRemove->next;
        if (nodeToRemove->next != NULL) nodeToRemove->next->previous = nodeToRemove->previous;
    }
    nodeToRemove->previous = NULL;
    nodeToRemove->next = NULL;
}

static void moveToHead(LRUCache *cacheObject, Node *nodeToMove)
{
    if (cacheObject->head == nodeToMove) return;
    removeNodeFromList(cacheObject, nodeToMove);
    insertAtHead(cacheObject, nodeToMove);
}

static Node* removeTailNode(LRUCache *cacheObject)
{
    if (cacheObject->tail == NULL) return NULL;
    Node *nodeToRemove = cacheObject->tail;
    if (cacheObject->head == cacheObject->tail)
    {
        cacheObject->head = NULL;
        cacheObject->tail = NULL;
    }
    else
    {
        cacheObject->tail = nodeToRemove->previous;
        if (cacheObject->tail != NULL) cacheObject->tail->next = NULL;
    }
    nodeToRemove->previous = NULL;
    nodeToRemove->next = NULL;
    return nodeToRemove;
}

void putValue(LRUCache *cacheObject, const int key, const char *value)
{
    if (cacheObject == NULL || value == NULL) return;
    int hashIndex = computeHashIndex(key, cacheObject->hashSize);
    Node *existingNode = cacheObject->hashMap[hashIndex];
    if (existingNode != NULL)
    {
        free(existingNode->data);
        existingNode->data = (char*)malloc(strlen(value) + 1);
        if (existingNode->data != NULL) strcpy(existingNode->data, value);
        moveToHead(cacheObject, existingNode);
    }
    else
    {
        Node *newNode = createNode(key, value);
        if (newNode == NULL) return;
        int isCacheFull = (cacheObject->currentSize == cacheObject->capacity);
        if (isCacheFull)
        {
            Node *evicted = removeTailNode(cacheObject);
            if (evicted != NULL)
            {
                int removedIndex = computeHashIndex(evicted->key, cacheObject->hashSize);
                cacheObject->hashMap[removedIndex] = NULL;
                if (evicted->data != NULL) free(evicted->data);
                free(evicted);
                cacheObject->currentSize--;
            }
        }
        insertAtHead(cacheObject, newNode);
        cacheObject->hashMap[hashIndex] = newNode;
        cacheObject->currentSize++;
    }
}

char* getValue(LRUCache *cacheObject, const int key)
{
    if (cacheObject == NULL) return "NULL";
    int hashIndex = computeHashIndex(key, cacheObject->hashSize);
    Node *foundNode = cacheObject->hashMap[hashIndex];
    if (foundNode != NULL)
    {
        moveToHead(cacheObject, foundNode);
        return foundNode->data;
    }
    return "NULL";
}

void freeCache(LRUCache *cacheObject)
{
    if (cacheObject == NULL) return;
    Node *current = cacheObject->head;
    while (current != NULL)
    {
        Node *nextNode = current->next;
        if (current->data != NULL) free(current->data);
        free(current);
        current = nextNode;
    }
    free(cacheObject->hashMap);
    free(cacheObject);
}
