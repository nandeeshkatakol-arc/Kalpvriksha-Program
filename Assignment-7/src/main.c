#include "../inc/app.h"

int main()
{
    initSystem();

    while (1)  
    {
        showMenu();
        handleChoice();
    }

    return 0;
}
