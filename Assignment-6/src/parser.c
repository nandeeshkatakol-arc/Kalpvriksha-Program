#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "directory.h"
#include "file.h"

static void trim(char *s)
{
    char *p = s;
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
    memmove(s, p, strlen(p) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
    {
        s[len - 1] = '\0';
        len--;
    }
    return;
}

void parseCommand(char *line)
{
    int continueExec = 1;
    char cmd[64];
    char rest[512];
    int n = 0;
    if (!line)
    {
        continueExec = 0;
    }
    if (continueExec)
    {
        trim(line);
        if (strlen(line) == 0)
        {
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        memset(cmd, 0, sizeof(cmd));
        memset(rest, 0, sizeof(rest));
        n = sscanf(line, "%63s %[^\n]", cmd, rest);
    }
    if (continueExec)
    {
        if (strcmp(cmd, "mkdir") == 0)
        {
            if (n < 2)
            {
                printf("Usage: mkdir <name>\n");
            }
            else
            {
                makeDirectory(rest);
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "ls") == 0)
        {
            listFiles();
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "cd") == 0)
        {
            if (n < 2)
            {
                printf("Usage: cd <name|..>\n");
            }
            else
            {
                changeDirectory(rest);
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "pwd") == 0)
        {
            showPath();
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "rmdir") == 0)
        {
            if (n < 2)
            {
                printf("Usage: rmdir <name>\n");
            }
            else
            {
                removeDirectory(rest);
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "create") == 0)
        {
            if (n < 2)
            {
                printf("Usage: create <name>\n");
            }
            else
            {
                createFile(rest);
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "write") == 0)
        {
            if (n < 2)
            {
                printf("Usage: write <name> \"content\"\n");
            }
            else
            {
                char namebuf[128];
                char content[512];
                memset(namebuf, 0, sizeof(namebuf));
                memset(content, 0, sizeof(content));
                int m = sscanf(rest, "%127s %[^\n]", namebuf, content);
                if (m < 2)
                {
                    printf("Usage: write <name> \"content\"\n");
                }
                else
                {
                    trim(content);
                    size_t len = strlen(content);
                    if (len > 1)
                    {
                        if (content[0] == '"' && content[len - 1] == '"')
                        {
                            content[len - 1] = '\0';
                            memmove(content, content + 1, strlen(content));
                        }
                    }
                    writeFile(namebuf, content);
                }
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "read") == 0)
        {
            if (n < 2)
            {
                printf("Usage: read <name>\n");
            }
            else
            {
                readFile(rest);
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "delete") == 0)
        {
            if (n < 2)
            {
                printf("Usage: delete <name>\n");
            }
            else
            {
                deleteFile(rest);
            }
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "df") == 0)
        {
            showDiskInfo();
            continueExec = 0;
        }
    }
    if (continueExec)
    {
        if (strcmp(cmd, "exit") == 0)
        {
            cleanupFileSystem();
            exit(0);
        }
    }
    if (continueExec)
    {
        if (strlen(cmd) > 0)
        {
            printf("Unknown command: %s\n", cmd);
        }
    }
    return;
}
