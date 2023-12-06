#include "input.h"
#include "int.h"
#include "position.h"
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

bool handleInput(struct ProgramStatus* status, struct SMenuItem* menu, bool columns)
{
    if(kbhit())
    {
        int key = checkKey();

        switch(key)
        {
            case KEY_QUIT:
                status->quit = true;
                status->errorLevel = ERRORLEVEL_EXIT_ERROR;
                break;
            case KEY_UP:
                updatePositionUp(&status->selected, &status->first, &status->last, columns);
                break;
            case KEY_DOWN:
                updatePositionDown(&status->selected, &status->first, &status->last, columns);
                break;
            case KEY_LEFT:
                updatePositionLeft(&status->selected, &status->first, &status->last, columns);
                break;
            case KEY_RIGHT:
                updatePositionRight(&status->selected, &status->first, &status->last, columns);
                break;
            case KEY_SELECT:
                runSelected(status->selected, menu, &status->errorLevel);
                status->quit = true;
                break;
            default:
                status->changeToIndex = handleAdditionalInput(key, &status->inputIndex);
                break;
        }

        return true;
    }

    return false;
}
