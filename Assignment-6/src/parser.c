#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

static void trim(char *s)
{
    char *p = s;
    while (isspace((unsigned char)*p))
    p++;
    memmove(s, p, strlen(p) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
    {
        s[len - 1] = '\0';
        --len;
    }
}

void parseCommand(char *line)
{
    if (!line) return;

    trim(line);
    if (strlen(line) == 0) return;

    char cmd[64] = {0};
    char rest[512] = {0};

    int n = sscanf(line, "%63s %[^\n]", cmd, rest);

    if (strcmp(cmd, "mkdir") == 0)
    {
        if (n < 2)
        {
            printf("Usage: mkdir <name>\n");
            return;
        }
        makeDirectory(rest);
        return;
    }

    if (strcmp(cmd, "ls") == 0)
    {
        listFiles();
        return;
    }

    if (strcmp(cmd, "cd") == 0)
    {
        if (n < 2)
        {
            printf("Usage: cd <name|..>\n");
            return;
        }
        changeDirectory(rest);
        return;
    }

    if (strcmp(cmd, "pwd") == 0)
    {
        showPath();
        return;
    }

    if (strcmp(cmd, "rmdir") == 0)
    {
        if (n < 2)
        {
            printf("Usage: rmdir <name>\n");
            return;
        }
        removeDirectory(rest);
        return;
    }

    if (strcmp(cmd, "create") == 0)
    {
        if (n < 2)
        {
            printf("Usage: create <name>\n");
            return;
        }
        createFile(rest);
        return;
    }

    if (strcmp(cmd, "write") == 0)
    {
        if (n < 2)
        {
            printf("Usage: write <name> \"content\"\n");
            return;
        }

        char name[128] = {0};
        char content[512] = {0};

        int m = sscanf(rest, "%127s %[^\n]", name, content);

        if (m < 2)
        {
            printf("Usage: write <name> \"content\"\n");
            return;
        }

        trim(content);

        if (content[0] == '"' && content[strlen(content) - 1] == '"')
        {
            content[strlen(content) - 1] = '\0';
            memmove(content, content + 1, strlen(content));
        }

        writeFile(name, content);
        return;
    }

    if (strcmp(cmd, "read") == 0)
    {
        if (n < 2)
        {
            printf("Usage: read <name>\n");
            return;
        }
        readFile(rest);
        return;
    }

    if (strcmp(cmd, "delete") == 0)
    {
        if (n < 2)
        {
            printf("Usage: delete <name>\n");
            return;
        }
        deleteFile(rest);
        return;
    }

    if (strcmp(cmd, "df") == 0)
    {
        showDiskInfo();
        return;
    }

    if (strcmp(cmd, "exit") == 0)
    {
        cleanupFileSystem();
        exit(0);
    }

    printf("Unknown command: %s\n", cmd);
}
