#include "int.h"
#include "position.h"
#include "copy_str.h"
#include "input.h"
#include "menu.h"
#include <string.h>
#include <conio.h>

static const char* FILE_OPTION    = "/file";
static const char* COLUMNS_OPTION = "/col";
static const char* ANSI_OPTION    = "/noansi";

struct SArguments
{
    unsigned fileNameIndex;
    bool noAnsi;
    bool columns;
};

void printHelp()
{
    printf("ZMENU\n");
    printf("Usage: zmenu.exe %s menu.txt [%s] [%s]\n\n", FILE_OPTION, ANSI_OPTION, COLUMNS_OPTION);
    printf("Keys:\n");
    printf("- Arrow keys to change selection\n");
    printf("- Number keys to jump to index\n");
    printf("- Enter to accept\n");
    printf("- Escape to exit\n\n");
}

bool parseArguments(int argc, char** argv, struct SArguments* arguments)
{
    const unsigned MIN_NUM_ARGS = 3;
    bool foundFileArg = false;

    if(argc < MIN_NUM_ARGS)
    {
        printHelp();
        return false;
    } else
    {
        int i;
        for(i = 1; i < argc; ++i)
        {
            if(!strcmp(FILE_OPTION, argv[i]) &&
                    argc > (i + 1) && !foundFileArg)
            {
                arguments->fileNameIndex = i + 1;
                foundFileArg = true;
            } else if(!strcmp(COLUMNS_OPTION, argv[i]))
            {
                arguments->columns = true;
                g_maxNameLength = MAX_NAME_LENGTH_2;
                g_maxNumItemsOnScreen = MAX_NUM_ITEMS_ON_SCREEN_2;
            } else if(!strcmp(ANSI_OPTION, argv[i]))
            {
                arguments->noAnsi = true;
            }
        }
    }

    if(!foundFileArg)
    {
        printf("ERROR: invalid arguments\n");
    }

    return foundFileArg;
}

void main(int argc, char* argv[])
{
    struct SMenuItem* menu = NULL;
    int errorLevel = ERRORLEVEL_EXIT_ERROR;
    index_t selected = 0;
    index_t first = 0;
    index_t last = 0;
    int key = 0;
    index_t inputIndex = 0;
    bool quit = false;
    struct SArguments arguments = { 0, false, false };

    // Flush printf without new line
    setbuf(stdout, NULL);
    
    if(parseArguments(argc, argv, &arguments) &&
            readMenuFromFile(argv[arguments.fileNameIndex], &menu))
    {
        screen(INT86_SET_TEXT_MODE);
        screen(INT86_HIDE_CURSOR);
        screen(INT86_SET_CURSOR_HOME);
        
        if(g_numItems < g_maxNumItemsOnScreen)
        {
            last = g_numItems - 1;
        } else
        {
            last = g_maxNumItemsOnScreen - 1;
        }

        printMenu(menu, selected, first, last, arguments.noAnsi, arguments.columns);
    } else
    {
        quit = true;
    }

    while(!quit)
    {
        if(kbhit())
        {
            bool changeToIndex = false;
            key = checkKey();

            switch(key)
            {
                case KEY_QUIT:
                    quit = true;
                    errorLevel = ERRORLEVEL_EXIT_ERROR;
                    break;
                case KEY_UP:
                    updatePositionUp(&selected, &first, &last, arguments.columns);
                    break;
                case KEY_DOWN:
                    updatePositionDown(&selected, &first, &last, arguments.columns);
                    break;
                case KEY_LEFT:
                    updatePositionLeft(&selected, &first, &last, arguments.columns);
                    break;
                case KEY_RIGHT:
                    updatePositionRight(&selected, &first, &last, arguments.columns);
                    break;
                case KEY_SELECT:
                    runSelected(selected, menu, &errorLevel);
                    quit = true;
                    break;
                default:
                    changeToIndex = handleAdditionalInput(key, &inputIndex);
                    break;
            }

            if(changeToIndex)
            {
                if(inputIndex < g_numItems)
                {
                    jumpToPosition(inputIndex, &selected, &first, &last, arguments.columns);
                }
            }

            inputIndex = 0;
            if(!quit)
            {
                screen(INT86_CLEAR_SCREEN);
                screen(INT86_SET_CURSOR_HOME);
                printMenu(menu, selected, first, last, arguments.noAnsi, arguments.columns);
            }
        }
    }
    
    // We should free the malloced memory
    // but when program exits, it's released anyway :)
    
    screen(INT86_RESTORE_CURSOR);
    exit(errorLevel);
}
