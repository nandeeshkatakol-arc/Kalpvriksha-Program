#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "vfs.h"
#include "file.h"
#include "parser.h"
#include "utils.h"

typedef void (*CommandHandler)(VFS *, int, char **);

typedef struct
{
    const char *name;
    CommandType type;
    int expectedArgs;
    CommandHandler handler;
} CommandEntry;

static void handleMkdir(VFS *vfs, int argc, char **argv)
{
    makeDirectory(vfs, argv[1]);
}

static void handleCreate(VFS *vfs, int argc, char **argv)
{
    createFile(vfs, argv[1]);
}

static void handleWrite(VFS *vfs, int argc, char **argv)
{
    char content[1024] = {0};
    extractQuotedContent(argv[2], content);
    writeFileToVFS(vfs, argv[1], content);
}

static void handleRead(VFS *vfs, int argc, char **argv)
{
    readFileFromVFS(vfs, argv[1]);
}

static void handleDelete(VFS *vfs, int argc, char **argv)
{
    deleteFileFromVFS(vfs, argv[1]);
}

static void handleRmdir(VFS *vfs, int argc, char **argv)
{
    removeDirectory(vfs, argv[1]);
}

static void handleLs(VFS *vfs, int argc, char **argv)
{
    listCurrentDirectory(vfs);
}

static void handleCd(VFS *vfs, int argc, char **argv)
{
    changeDirectory(vfs, argv[1]);
}

static void handlePwd(VFS *vfs, int argc, char **argv)
{
    printPath(vfs->cwd);
}

static void handleDf(VFS *vfs, int argc, char **argv)
{
    showDiskInfo(vfs);
}

static void handleExit(VFS *vfs, int argc, char **argv)
{
    destroyVfs(vfs);
    printf("Memory released. Exiting program...\n");
    exit(0);
}

static CommandEntry commandTable[] =
{
    {"mkdir",  CMD_MKDIR,  2, handleMkdir},
    {"create", CMD_CREATE, 2, handleCreate},
    {"write",  CMD_WRITE,  3, handleWrite},
    {"read",   CMD_READ,   2, handleRead},
    {"delete", CMD_DELETE, 2, handleDelete},
    {"rmdir",  CMD_RMDIR,  2, handleRmdir},
    {"ls",     CMD_LS,     1, handleLs},
    {"cd",     CMD_CD,     2, handleCd},
    {"pwd",    CMD_PWD,    1, handlePwd},
    {"df",     CMD_DF,     1, handleDf},
    {"exit",   CMD_EXIT,   1, handleExit}
};

void trimWhitespace(char *str)
{
    if (!str)
    {
        return;
    }

    int len = (int)strlen(str);
    int start = 0;

    while (isspace((unsigned char)str[start]))
    {
        start++;
    }

    int end = len - 1;
    while (end >= start && isspace((unsigned char)str[end]))
    {
        end--;
    }

    int newLen = end - start + 1;
    memmove(str, str + start, newLen);
    str[newLen] = '\0';
}

void extractQuotedContent(const char *input, char *output)
{
    const char *start = strchr(input, '"');
    if (!start)
    {
        output[0] = '\0';
        return;
    }

    start++;
    const char *end = strchr(start, '"');
    if (!end)
    {
        output[0] = '\0';
        return;
    }

    int len = (int)(end - start);
    strncpy(output, start, len);
    output[len] = '\0';
}

void tokenize(char *input, char *argv[], int *argc)
{
    *argc = 0;
    char *token = strtok(input, " ");

    while (token != NULL && *argc < 10)
    {
        argv[*argc] = token;
        (*argc)++;
        token = strtok(NULL, " ");
    }
}

void parseCommand(VFS *vfs, char *input)
{
    trimWhitespace(input);

    if (strlen(input) == 0)
    {
        return;
    }

    if (strncmp(input, "write ", 6) == 0)
    {
        char filename[128] = {0};
        char content[1024] = {0};

        sscanf(input + 6, "%127s", filename);
        extractQuotedContent(input, content);
        writeFileToVFS(vfs, filename, content);
        return;
    }

    char buffer[512];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *argv[10];
    int argc = 0;

    tokenize(buffer, argv, &argc);

    if (argc == 0)
    {
        return;
    }

    for (int i = 0; i < (int)(sizeof(commandTable) / sizeof(CommandEntry)); i++)
    {
        if (strcmp(argv[0], commandTable[i].name) == 0)
        {
            if (argc != commandTable[i].expectedArgs)
            {
                printf("Usage: %s <args>\n", commandTable[i].name);
                return;
            }

            commandTable[i].handler(vfs, argc, argv);
            return;
        }
    }

    printf("Unknown command: %s\n", argv[0]);
}
