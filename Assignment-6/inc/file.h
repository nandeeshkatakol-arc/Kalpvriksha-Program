#ifndef FILE_H
#define FILE_H

#include "directory.h"

void createFile(const char *name);
void writeFile(const char *name, const char *data);
void readFile(const char *name);
void deleteFile(const char *name);

#endif
