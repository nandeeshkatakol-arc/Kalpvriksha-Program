#include <stdio.h>
#include <string.h>
#include "LRUcache.h"

int main(void)
{
    LRUCache *cacheObject = NULL;
    char commandBuffer[64];
    int keyValue = 0;
    int capacityValue = 0;
    char valueBuffer[2048];
    char *returnedValue = NULL;
    int continueRunning = 1;

    while (continueRunning && scanf("%63s", commandBuffer) == 1)
    {
        if (strcmp(commandBuffer, "createCache") == 0)
        {
            if (scanf("%d", &capacityValue) == 1)
            {
                if (cacheObject != NULL)
                {
                    freeCache(cacheObject);
                    cacheObject = NULL;
                }
                cacheObject = createCache(capacityValue);
            }
        }
        else if (strcmp(commandBuffer, "put") == 0)
        {
            if (scanf("%d %2047s", &keyValue, valueBuffer) == 2)
            {
                if (cacheObject != NULL)
                {
                    putValue(cacheObject, keyValue, valueBuffer);
                }
            }
        }
        else if (strcmp(commandBuffer, "get") == 0)
        {
            if (scanf("%d", &keyValue) == 1)
            {
                if (cacheObject != NULL)
                {
                    returnedValue = getValue(cacheObject, keyValue);
                    if (returnedValue != NULL)
                    {
                        printf("%s\n", returnedValue);
                    }
                    else
                    {
                        printf("NULL\n");
                    }
                }
                else
                {
                    printf("NULL\n");
                }
            }
        }
        else if (strcmp(commandBuffer, "exit") == 0)
        {
            continueRunning = 0;
        }
        else
        {
            int ch = 0;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }
        }
    }

    if (cacheObject != NULL)
    {
        freeCache(cacheObject);
        cacheObject = NULL;
    }
    return 0;
}
