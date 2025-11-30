#include "LRUcache.h"

static int computeHashIndex(const int key, const int hashSize)
{
    int index = 0;
    if (hashSize != 0)
    {
        index = key % hashSize;
        if (index < 0)
        {
            index = -index;
        }
    }
    return index;
}

static Node* createNodeInternal(const int key, const char *data)
{
    Node *resultNode = NULL;
    char *tempData = NULL;
    int success = 0;

    resultNode = (Node*)malloc(sizeof(Node));
    if (resultNode != NULL)
    {
        resultNode->key = key;
        resultNode->previous = NULL;
        resultNode->next = NULL;
        resultNode->data = NULL;
        if (data != NULL)
        {
            size_t dataLength = strlen(data);
            tempData = (char*)malloc(dataLength + 1);
            if (tempData != NULL)
            {
                strcpy(tempData, data);
                resultNode->data = tempData;
                tempData = NULL;
                success = 1;
            }
        }
        else
        {
            success = 1;
        }
    }

    if (!success)
    {
        if (tempData != NULL)
        {
            free(tempData);
            tempData = NULL;
        }
        if (resultNode != NULL)
        {
            free(resultNode);
            resultNode = NULL;
        }
    }

    return resultNode;
}

LRUCache* createCache(const int capacity)
{
    LRUCache *createdCache = NULL;
    int success = 0;

    if (capacity > 0)
    {
        createdCache = (LRUCache*)malloc(sizeof(LRUCache));
        if (createdCache != NULL)
        {
            createdCache->capacity = capacity;
            createdCache->currentSize = 0;
            createdCache->head = NULL;
            createdCache->tail = NULL;
            createdCache->hashSize = capacity * 2;
            if (createdCache->hashSize > 0)
            {
                createdCache->hashMap = (Node**)calloc(createdCache->hashSize, sizeof(Node*));
                if (createdCache->hashMap != NULL)
                {
                    success = 1;
                }
            }
        }
    }

    if (!success)
    {
        if (createdCache != NULL)
        {
            free(createdCache);
            createdCache = NULL;
        }
    }

    return createdCache;
}

static void insertAtHead(LRUCache *cacheObject, Node *newNode)
{
    if (cacheObject != NULL && newNode != NULL)
    {
        if (cacheObject->head == NULL)
        {
            cacheObject->head = newNode;
            cacheObject->tail = newNode;
            newNode->previous = NULL;
            newNode->next = NULL;
        }
        else
        {
            newNode->next = cacheObject->head;
            newNode->previous = NULL;
            cacheObject->head->previous = newNode;
            cacheObject->head = newNode;
        }
    }
    return;
}

static void removeNodeFromList(LRUCache *cacheObject, Node *nodeToRemove)
{
    if (cacheObject != NULL && nodeToRemove != NULL)
    {
        if (nodeToRemove == cacheObject->head)
        {
            cacheObject->head = nodeToRemove->next;
            if (cacheObject->head != NULL)
            {
                cacheObject->head->previous = NULL;
            }
        }
        else if (nodeToRemove == cacheObject->tail)
        {
            cacheObject->tail = nodeToRemove->previous;
            if (cacheObject->tail != NULL)
            {
                cacheObject->tail->next = NULL;
            }
        }
        else
        {
            if (nodeToRemove->previous != NULL)
            {
                nodeToRemove->previous->next = nodeToRemove->next;
            }
            if (nodeToRemove->next != NULL)
            {
                nodeToRemove->next->previous = nodeToRemove->previous;
            }
        }
        nodeToRemove->previous = NULL;
        nodeToRemove->next = NULL;
    }
    return;
}

static void moveToHead(LRUCache *cacheObject, Node *nodeToMove)
{
    if (cacheObject != NULL && nodeToMove != NULL)
    {
        if (cacheObject->head != nodeToMove)
        {
            removeNodeFromList(cacheObject, nodeToMove);
            insertAtHead(cacheObject, nodeToMove);
        }
    }
    return;
}

static Node* removeTailNode(LRUCache *cacheObject)
{
    Node *removedNode = NULL;
    if (cacheObject != NULL)
    {
        if (cacheObject->tail != NULL)
        {
            removedNode = cacheObject->tail;
            if (cacheObject->head == cacheObject->tail)
            {
                cacheObject->head = NULL;
                cacheObject->tail = NULL;
            }
            else
            {
                cacheObject->tail = removedNode->previous;
                if (cacheObject->tail != NULL)
                {
                    cacheObject->tail->next = NULL;
                }
            }
            removedNode->previous = NULL;
            removedNode->next = NULL;
        }
    }
    return removedNode;
}

void putValue(LRUCache *cacheObject, const int key, const char *value)
{
    if (cacheObject != NULL && value != NULL)
    {
        int hashIndex = computeHashIndex(key, cacheObject->hashSize);
        Node *existingNode = cacheObject->hashMap[hashIndex];
        if (existingNode != NULL)
        {
            char *replacementBuffer = NULL;
            size_t newLength = strlen(value);
            replacementBuffer = (char*)malloc(newLength + 1);
            if (replacementBuffer != NULL)
            {
                strcpy(replacementBuffer, value);
                if (existingNode->data != NULL)
                {
                    free(existingNode->data);
                }
                existingNode->data = replacementBuffer;
                replacementBuffer = NULL;
            }
            moveToHead(cacheObject, existingNode);
        }
        else
        {
            Node *newNode = createNodeInternal(key, value);
            if (newNode != NULL)
            {
                if (cacheObject->currentSize == cacheObject->capacity)
                {
                    Node *evictedNode = removeTailNode(cacheObject);
                    if (evictedNode != NULL)
                    {
                        int removedIndex = computeHashIndex(evictedNode->key, cacheObject->hashSize);
                        cacheObject->hashMap[removedIndex] = NULL;
                        if (evictedNode->data != NULL)
                        {
                            free(evictedNode->data);
                        }
                        free(evictedNode);
                        cacheObject->currentSize--;
                    }
                }
                insertAtHead(cacheObject, newNode);
                cacheObject->hashMap[hashIndex] = newNode;
                cacheObject->currentSize++;
            }
        }
    }
    return;
}

char* getValue(LRUCache *cacheObject, const int key)
{
    char *resultData = NULL;
    if (cacheObject != NULL)
    {
        int hashIndex = computeHashIndex(key, cacheObject->hashSize);
        Node *foundNode = cacheObject->hashMap[hashIndex];
        if (foundNode != NULL)
        {
            moveToHead(cacheObject, foundNode);
            resultData = foundNode->data;
        }
    }
    return resultData;
}

void freeCache(LRUCache *cacheObject)
{
    if (cacheObject != NULL)
    {
        Node *currentNode = cacheObject->head;
        while (currentNode != NULL)
        {
            Node *nextNode = currentNode->next;
            if (currentNode->data != NULL)
            {
                free(currentNode->data);
            }
            free(currentNode);
            currentNode = nextNode;
        }
        if (cacheObject->hashMap != NULL)
        {
            free(cacheObject->hashMap);
        }
        free(cacheObject);
    }
    return;
}
