#include <stdio.h>
#include <string.h>
#include "LRUcache.h"

int main()
{
    LRUCache *cacheObject = NULL;
    char command[50];
    int key = 0;
    int capacity = 0;
    char value[200];
    char *returnedValue = NULL;
    int shouldContinue = 1;

    while (shouldContinue)
    {
        if (scanf("%49s", command) != 1) break;
        if (strcmp(command, "createCache") == 0)
        {
            if (scanf("%d", &capacity) == 1)
            {
                if (cacheObject != NULL) freeCache(cacheObject);
                cacheObject = createCache(capacity);
            }
        }
        else if (strcmp(command, "put") == 0)
        {
            if (scanf("%d %199s", &key, value) == 2)
            {
                if (cacheObject != NULL) putValue(cacheObject, key, value);
            }
        }
        else if (strcmp(command, "get") == 0)
        {
            if (scanf("%d", &key) == 1)
            {
                if (cacheObject != NULL)
                {
                    returnedValue = getValue(cacheObject, key);
                    printf("%s\n", returnedValue);
                }
                else
                {
                    printf("NULL\n");
                }
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            shouldContinue = 0;
        }
    }

    if (cacheObject != NULL) freeCache(cacheObject);
    return 0;
}
