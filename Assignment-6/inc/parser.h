#ifndef PARSER_H
#define PARSER_H

#include "vfs.h"

typedef enum
{
    CMD_INVALID = -1,
    CMD_MKDIR,
    CMD_CREATE,
    CMD_WRITE,
    CMD_READ,
    CMD_DELETE,
    CMD_RMDIR,
    CMD_LS,
    CMD_CD,
    CMD_PWD,
    CMD_DF,
    CMD_EXIT
} CommandType;

void trimWhitespace(char *str);
void extractQuotedContent(const char *input, char *output);
void tokenize(char *input, char *argv[], int *argc);

void parseCommand(VFS *vfs, char *input);

#endif
