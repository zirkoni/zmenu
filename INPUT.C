#include "input.h"
#include "int.h"
#include "position.h"
#include <conio.h>
#include <string.h>

#define MAX_SEARCH_LENGTH 32

#define KEY_SELECT           13
#define KEY_QUIT             27
#define KEY_UP              328
#define KEY_DOWN            336
#define KEY_LEFT            331
#define KEY_RIGHT           333
#define KEY_BACKSPACE         8
#define KEY_SEARCH          115 // S
#define KEY_SEARCH_AGAIN    317 // F3

int searchLength = 0;
char searchString[MAX_SEARCH_LENGTH] = {0};

void printSearchInput(const char* searchString)
{
    screen(INT86_SET_CURSOR_HOME);
    printf("Search: %s ", searchString);
}

void printSearchNotFound(const char* searchString)
{
    screen(INT86_SET_CURSOR_HOME);
    printf("Unable to find \"%s\"", searchString);
}

void appendSearchString(int key, int* len, char* searchString)
{
    if(key >= ' ' && key <= '~' && *len < MAX_SEARCH_LENGTH)
    {
        searchString[*len] = key;
        (*len)++;
    } else if(key == KEY_BACKSPACE && *len > 0)
    {
        (*len)--;
        searchString[*len] = '\0';
    }
}

void printGotoInput(index_t* inputIndex)
{
    screen(INT86_SET_CURSOR_HOME);
    printf("Goto index: %lu ", *inputIndex);
}

void appendGotoIndex(int key, index_t* inputIndex)
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

void updateSearchPosition(struct ProgramStatus* status, bool columns)
{
    if(!columns)
    {
        updatePositionDown(&status->selected, &status->first, &status->last, columns);
    } else
    {
        if(status->selected % 2)
        {
            updatePositionLeft(&status->selected, &status->first, &status->last, columns);
            updatePositionDown(&status->selected, &status->first, &status->last, columns);
        } else
        {
            if(status->selected != g_numItems - 1)
            {
                updatePositionRight(&status->selected, &status->first, &status->last, columns);
            } else
            {
                updatePositionDown(&status->selected, &status->first, &status->last, columns);
            }
        }
    }
}

void doSearch(const char* searchString, struct ProgramStatus* status, struct SMenuItem* menu, bool columns, bool isNew)
{
    char* resultPtr = NULL;
    index_t oldSelected = status->selected;
    index_t oldFirst = status->first;
    index_t oldLast = status->last;
    bool found = false;

    struct SMenuItem* item = menu;
    index_t searchIdx = 0;
    index_t searchEnd;

    // Start from the next item if we are repeating search
    if(!isNew)
    {
        updateSearchPosition(status, columns);
    }

    if(status->selected != 0)
    {
        searchEnd = status->selected - 1;
    } else
    {
        searchEnd = g_numItems - 1;
    }

    while(searchIdx != status->selected)
    {
        ++searchIdx;
        item = item->next;
    }

    while(true)
    {
        resultPtr = strstr(item->name, searchString);

        if(resultPtr != NULL)
        {
            found = true;
            break;
        }

        if(searchIdx == searchEnd)
        {
            break;
        }

        updateSearchPosition(status, columns);
        searchIdx = status->selected;

        if(searchIdx != 0)
        {
            item = item->next;
        } else
        {
            item = menu;
        }
    }

    if(!found)
    {
        printSearchNotFound(searchString);
        status->selected = oldSelected;
        status->first = oldFirst;
        status->last = oldLast;
        while(!kbhit()) {}
        checkKey(); // Remove pressed key from buffer
    }
}

bool handleAdditionalInput(int key, struct ProgramStatus* status, struct SMenuItem* menu, bool columns)
{
    bool changeToIndex = false;
    bool search = false;

    if(key == KEY_SEARCH)
    {
        search = true;
        printSearchInput(searchString);
    } else if(key == KEY_SEARCH_AGAIN && searchLength > 0)
    {
        doSearch(searchString, status, menu, columns, false);
        return false;
    } else
    {
        appendGotoIndex(key, &status->inputIndex);
        printGotoInput(&status->inputIndex);
    }

    while(true)
    {
        if(kbhit())
        {
            key = checkKey();

            if(key == KEY_QUIT)
            {
                break;
            } else if(key == KEY_SELECT && search)
            {
                doSearch(searchString, status, menu, columns, true);
                break;
            } else if(key == KEY_SELECT)
            {
                changeToIndex = true;
                break;
            } else if(search)
            {
                appendSearchString(key, &searchLength, searchString);
                printSearchInput(searchString);
            } else
            {
                appendGotoIndex(key, &status->inputIndex);
                printGotoInput(&status->inputIndex);
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
                status->changeToIndex = handleAdditionalInput(key, status, menu, columns);
                break;
        }

        return true;
    }

    return false;
}

void clearKeyboardBuffer()
{
    while(kbhit())
    {
        checkKey();
    }
}
