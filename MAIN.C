#include "int.h"
#include "position.h"
#include "copy_str.h"
#include "input.h"
#include <string.h>

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
    struct SArguments arguments = { 0, false, false };
    struct ProgramStatus status;
    status.quit = false;
    status.changeToIndex = false;
    status.errorLevel = ERRORLEVEL_EXIT_ERROR;
    status.selected = 0;
    status.first = 0;
    status.last = 0;
    status.inputIndex = 0;

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
            status.last = g_numItems - 1;
        } else
        {
            status.last = g_maxNumItemsOnScreen - 1;
        }

        printMenu(menu, status.selected, status.first, status.last, arguments.noAnsi, arguments.columns);
    } else
    {
        status.quit = true;
    }

    clearKeyboardBuffer();

    while(!status.quit)
    {
        if(handleInput(&status, menu, arguments.columns))
        {
            if(status.changeToIndex)
            {
                status.changeToIndex = false;
                if(status.inputIndex < g_numItems)
                {
                    jumpToPosition(status.inputIndex, &status.selected, &status.first, &status.last, arguments.columns);
                }
            }

            status.inputIndex = 0;
            if(!status.quit)
            {
                screen(INT86_CLEAR_SCREEN);
                screen(INT86_SET_CURSOR_HOME);
                printMenu(menu, status.selected, status.first, status.last, arguments.noAnsi, arguments.columns);
            }
        }
    }

    // We should free the malloced memory
    // but when program exits, it's released anyway :)

    screen(INT86_RESTORE_CURSOR);
    exit(status.errorLevel);
}
