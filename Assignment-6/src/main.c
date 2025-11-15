#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "directory.h"

int main()
{
    setupFileSystem();
    char line[512];
    int running = 1;
    while (running)
    {
        if (cwd)
        {
            printf("%s> ", cwd->name);
        }
        else
        {
            printf("/> ");
        }
        if (!fgets(line, sizeof(line), stdin))
        {
            running = 0;
        }
        else
        {
            line[strcspn(line, "\n")] = '\0';
            parseCommand(line);
        }
    }
    cleanupFileSystem();
    return 0;
}
