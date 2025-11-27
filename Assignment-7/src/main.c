#include "ODI_PlayerAnalyzer.h"
#include <stdio.h>

int main()
{
    initSystem();

    while (1)  
    {
        printf("\n===== ICC ODI Player Performance Analyzer =====\n");
        showMenu();
        handleChoice();
    }

    return 0;
}
