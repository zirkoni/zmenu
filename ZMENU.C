#include "int.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_NAME_LENGTH_1         60
#define MAX_NAME_LENGTH_2         38
#define MAX_PATH_LENGTH           36
#define MAX_EXE_LENGTH            12
#define MAX_NUM_ITEMS_ON_SCREEN_1 23
#define MAX_NUM_ITEMS_ON_SCREEN_2 46

#define ERRORLEVEL_CONTINUE   0
#define ERRORLEVEL_EXIT_ERROR 1

static const char* FILE_OPTION    = "/file";
static const char* COLUMNS_OPTION = "/col";
static const char* ANSI_OPTION    = "/noansi";

typedef long unsigned index_t;

index_t g_numItems = 0;
size_t g_maxNameLength = MAX_NAME_LENGTH_1;
size_t g_maxNumItemsOnScreen = MAX_NUM_ITEMS_ON_SCREEN_1;

struct SArguments
{
    unsigned fileNameIndex;
    bool noAnsi;
    bool columns;
};

struct SMenuItem
{
    char* name;
    char* path;
    char* executable;

    struct SMenuItem* next;
};

// TODO: all declarations here or header file + multiple c-files?
void setCursorPosition(unsigned short x, unsigned short y);

struct SMenuItem* newMenuItem()
{
    struct SMenuItem* item = malloc(sizeof(struct SMenuItem));
    if(item != NULL)
    {
        item->name = NULL;
        item->path = NULL;
        item->executable = NULL;
        item->next = NULL;
    }

    return item;
}


void printHelp()
{
    printf("ZMENU\n");
    printf("Usage: zmenu.exe %s menu.txt [%s]\n\n", FILE_OPTION, ANSI_OPTION);
    printf("Keys:\n");
    printf("Up/Down arrow keys to change selection.\n");
    printf("Number keys to jump to index.\n");
    printf("Enter to accept.\n");
    printf("Escape to exit.\n\n");
    printf("menu.txt entry example (character limits %u, %u and %u):\n",
            g_maxNameLength - 1, MAX_PATH_LENGTH - 1, MAX_EXE_LENGTH - 1);
    printf("Commander Keen 1: Marooned on Mars\n");
    printf("c:\\keen1\n");
    printf("keen1.exe\n\n");
    printf("Output on screen:\n");
    printf("0. Commander Keen 1: Marooned on Mars\n");
}

void printMenu(struct SMenuItem* menu, int selected,
        int first, int last, bool noAnsi, bool twoColumns)
{
    int i = 0;
    unsigned short xOffset = 0;
    unsigned short yOffset = 0;
    struct SMenuItem* item = menu;

    while(i < first)
    {
        item = item->next;
        ++i;
    }

    while(i <= last)
    {
        xOffset = twoColumns * (i % 2) * (g_maxNameLength + 2);
        yOffset = (i - first) / (2 - !twoColumns) + 1;

        setCursorPosition(yOffset, xOffset);

        if(i != selected)
        {
            printf(" %s", item->name);
        } else
        {
            if(!noAnsi)
            {
                printf(" \033[7m%s\033[0m", item->name);
            } else
            {
                printf("[%s]", item->name);
            }
        }

        item = item->next;
        ++i;
    }
}

int numDigits(index_t number)
{
    int result = 1;
    while(number /= 10)
    {
        ++result;
    }

    return result;
}

bool copyStringWithPrefix(char** to, char* from, size_t size,
        size_t maxSize, int endOfFile, bool usePrefix, index_t prefix)
{
    int offset = 0;
    size_t realSize = size;

    if(usePrefix)
    {
        offset = numDigits(prefix) + 2; // dot + space = +2
        realSize = realSize + offset;
    }

    if(endOfFile) ++realSize;
    if(realSize > maxSize)
    {
        realSize = maxSize;
        size = realSize - offset;
    }

    (*to) = malloc(realSize);

    if((*to) != NULL)
    {
        if(usePrefix)
        {
            sprintf((*to), "%lu. ", g_numItems);
        }

        memcpy((*to) + offset, from, size);
        (*to)[realSize - 1] = 0;
    } else
    {
        printf("ERROR: malloc failed. Menu too big?\n");
        return false;
    }

    return true;
}

bool copyString(char** to, char* from, size_t size,
        size_t maxSize, int endOfFile)
{
    return copyStringWithPrefix(to, from, size, maxSize, endOfFile, false, 0);
}

bool copyName(char** to, char* from, size_t size, int endOfFile)
{
    return copyStringWithPrefix(to, from, size, g_maxNameLength, endOfFile, true, g_numItems);
}

bool readMenuFromFile(char* fileName, struct SMenuItem** menu)
{
    bool readFileOk = true;
    FILE* file = fopen(fileName, "r");
    char line[128] = { 0 };

    if(file != NULL)
    {
        struct SMenuItem* item = NULL;
        struct SMenuItem** next = NULL;

        while(fgets(line, sizeof(line), file))
        {
            size_t len = strlen(line);

            if(item == NULL)
            {
                item = newMenuItem();
                if(item == NULL)
                {
                    readFileOk = false;
                    printf("ERROR: malloc failed\n");
                    break;
                }
            }

            if(len > 1)
            {
            
                if(item->name == NULL)
                {
                    readFileOk = copyName(&(item->name), line, len, feof(file));
                } else if(item->path == NULL)
                {
                    readFileOk = copyString(&(item->path), line, len,
                        MAX_PATH_LENGTH, feof(file));
                } else
                {
                    readFileOk = copyString(&(item->executable), line, len,
                        MAX_EXE_LENGTH, feof(file));

                    if(*menu == NULL)
                    {
                        *menu = item;
                        next = &((*menu)->next);
                    } else
                    {
                        (*next) = item;
                        next = &(item->next);
                    }

                    item = NULL;
                    g_numItems = g_numItems + 1;
                }

                if(!readFileOk)
                {
                    break;
                }
            }
        }

        fclose(file);
    } else
    {
        readFileOk = false;
        printf("ERROR: Unable to open file \"%s\"\n", fileName);
    }

    return readFileOk;
}

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

void updatePositionUp(index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    const unsigned step = 1 + twoColumns;

    if(*selected >= step)
    {
        *selected = (*selected) - step;
    } else
    {
        if(g_numItems % 2 == 0)
        {
            const unsigned correction = twoColumns * (*selected % 2 ? 0 : 1);
            *selected = g_numItems - 1 - correction;
            *last = *selected + correction;
        } else
        {
            const unsigned correction = twoColumns * (*selected % 2 ? 1 : 0);
            *selected = g_numItems - 1 - correction;
            *last = *selected + correction;
        }

        if(g_numItems >= g_maxNumItemsOnScreen)
        {
            *first = g_numItems - g_maxNumItemsOnScreen;
            *first += twoColumns * (*first & 1); // Round up to even number (first is always even in 2 column mode)
        } else
        {
            *first = 0;
        }
    }

    if(*selected < *first)
    {
        *first = *first - step;

        if(g_numItems % 2 != 0 && *last == (g_numItems - 1))
        {
            *last = *last - 1;
        } else
        {
            *last = *last - step;
        }
    }
}

void updatePositionDown(index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    const unsigned step = 1 + twoColumns;
    *selected = (*selected) + step;

    if(*selected > *last)
    {
        *first += step;
        *last = *selected + twoColumns * (*selected % 2 ? 0 : 1);
    }

    if(*selected >= g_numItems)
    {
        *selected = 0 + twoColumns * (*selected % 2 ? 1 : 0);
        *first = 0;

        if(g_numItems < g_maxNumItemsOnScreen)
        {
            *last = g_numItems - 1;
        } else
        {
            *last = g_maxNumItemsOnScreen - 1;
        }
    }

    if(*last >= g_numItems)
    {
        *last = g_numItems - 1;
    }
}

void updatePositionLeft(index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    if(twoColumns && (*selected % 2))
    {
        (*selected)--;
    }
}

void updatePositionRight(index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    if(twoColumns && !(*selected % 2))
    {
        if(*selected < *last)
        {
            (*selected)++;
        }
    }
}

void jumpToPosition(index_t inputIndex, index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    while(inputIndex != *selected)
    {
        if(inputIndex > *selected)
        {
            updatePositionDown(selected, first, last, twoColumns);
            
            // We can overshoot in 2 columns mode
            if(inputIndex < *selected || (inputIndex == (g_numItems - 1) && *selected == (inputIndex - 1)))
            {
                *selected = inputIndex;

                // But then first and last item visible might need to be updated
                if(*selected > *last)
                {
                    (*first) = (*first) + 1 + twoColumns;
                    (*last) = (*last) + 1 + twoColumns;
                    if(*last > g_numItems - 1)
                    {
                        *last = g_numItems - 1;
                    }
                }
            }

        } else if(inputIndex < *selected)
        {
            updatePositionUp(selected, first, last, twoColumns);

            if(inputIndex > *selected || inputIndex == 0 && *selected == 1)
            {
                *selected = inputIndex;

                if(!twoColumns && *selected < *first)
                {
                    (*first) = *selected;
                    (*last) = (*last) - 1;
                }
            }
        }
    }
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

void runSelected(index_t selected, struct SMenuItem* menu, int* errorLevel)
{
    index_t i;
    struct SMenuItem* item = menu;
    const char* RUN_FILE = "ZZZ_TMP.BAT";

    for(i = 0; i <= selected; ++i)
    {
        if(i == selected)
        {
            FILE* file;
            file = fopen(RUN_FILE, "w+");

            if(file != NULL)
            {
                fprintf(file, "set errorlevel=%u\n", ERRORLEVEL_EXIT_ERROR);
                fprintf(file, "if not exist %s\\%s goto not_exist\n",
                        item->path, item->executable);
                fprintf(file, "cd %s\n", item->path);
                fprintf(file, "call %s\n", item->executable);
                fprintf(file, "set errorlevel=%u\n", ERRORLEVEL_CONTINUE);
                fprintf(file, "goto end\n");
                fprintf(file, ":not_exist\n");
                fprintf(file, "@echo ERROR: file %s\\%s does not exist\n",
                        item->path, item->executable);
                fprintf(file, ":end\n");
                fclose(file);
                *errorLevel = ERRORLEVEL_CONTINUE;
                break;
            } else
            {
                printf("ERROR: file create failed %s\n", RUN_FILE);
            }
        }

        item = item->next;
    }
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
