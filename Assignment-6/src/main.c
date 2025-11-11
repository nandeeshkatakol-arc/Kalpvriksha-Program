#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"
#include "parser.h"
#include "utils.h"

int main()
{
    int totalBlocks = 1024;
    int blockSize = 512;

    printLine();
    printf("Compact Virtual File System Simulation\n");
    printLine();

    VFS *vfs = createVfs(totalBlocks, blockSize);
    if (!vfs)
    {
        fprintf(stderr, "Failed to initialize VFS.\n");
        return EXIT_FAILURE;
    }
    char input[256];
    while (1)
    {
        printf("%s> ", vfs->cwd->name);

        if (!fgets(input, sizeof(input), stdin))
        {
            printf("\nInput error. Exiting...\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        parseCommand(vfs, input);
    }

    destroyVfs(vfs);
    return 0;
}