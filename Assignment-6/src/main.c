#include <stdio.h>
#include <string.h>
#include "commandParser.h"
#include "directory.h"

int main(void)
{
    setupFileSystem();

    char line[512];

    while (1)
    {
        printf("%s> ", cwd ? cwd->name : "/");
        if (!fgets(line, sizeof(line), stdin))
        {
            break;
        }

        line[strcspn(line, "\n")] = '\0';
        parseCommand(line);
    }

    cleanupFileSystem();
    return 0;
}
