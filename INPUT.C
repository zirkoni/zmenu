#include "input.h"
#include "int.h"
#include <conio.h>

void printInput(index_t* inputIndex)
{
    screen(INT86_SET_CURSOR_HOME);
    printf("Goto index: %lu          \n", *inputIndex); // Write spaces to overwrite deleted (backspace) digits
}

void appendInputIndex(int key, index_t* inputIndex)
{
    if(key >= '0' && key <= '9')
    {
        index_t newValue = 0;
        newValue = *inputIndex * 10 + key - '0';
        if(newValue < g_numItems)
        {
            *inputIndex = newValue;
        }
    } else if(key == KEY_BACKSPACE)
    {
        *inputIndex = *inputIndex / 10;
    }
}

bool handleAdditionalInput(int key, index_t* inputIndex)
{
    bool changeToIndex = false;
    appendInputIndex(key, inputIndex);
    printInput(inputIndex);

    while(true)
    {
        if(kbhit())
        {
            key = checkKey();

            if(key == KEY_QUIT)
            {
                break;
            } else if(key == KEY_SELECT)
            {
                changeToIndex = true;
                break;
            } else
            {
                appendInputIndex(key, inputIndex);
                printInput(inputIndex);
            }
        }
    }

    return changeToIndex;
}

