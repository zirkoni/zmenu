#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <i86.h>
#include <conio.h>

#define KEY_SELECT 13
#define KEY_QUIT   27
#define KEY_UP     328
#define KEY_DOWN   336

#define MAX_NAME_LENGTH    60
#define MAX_PATH_LENGTH    36
#define MAX_EXE_LENGTH     12
#define MAX_NUM_MENU_ITEMS 23

#define INT86_SET_TEXT_MODE   0
#define INT86_SET_CURSOR_HOME 1
#define INT86_HIDE_CURSOR     2
#define INT86_CLEAR_SCREEN    3
#define INT86_RESTORE_CURSOR  4

#define ERRORLEVEL_CONTINUE   0
#define ERRORLEVEL_EXIT_ERROR 1

static const char* FILE_OPTION = "/file";
static const char* ANSI_OPTION = "/noansi";

struct SMenuItem
{
    char* name;
    char* path;
    char* executable;

    struct SMenuItem* next;
};

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
    printf("Enter to accept.\n");
    printf("Escape to exit.\n\n");
    printf("menu.txt entry example (character limits %u, %u and %u):\n", MAX_NAME_LENGTH - 1, MAX_PATH_LENGTH - 1, MAX_EXE_LENGTH - 1);
    printf("Commander Keen 1: Marooned on Mars\n");
    printf("c:\\keen1\n");
    printf("keen1.exe\n");
}

void printMenu(struct SMenuItem* menu, int selected,
        int first, int last, bool noAnsi)
{
    int i = 0;
    struct SMenuItem* item = menu;

    while(i < first)
    {
        item = item->next;
        ++i;
    }

    printf("\n");
    while(i <= last)
    {
        if(i != selected)
        {
            printf(" %s\n", item->name);
        } else
        {
            if(!noAnsi)
            {
                printf(" \033[7m%s\n\033[0m", item->name);
            } else
            {
                printf("[%s]\n", item->name);
            }
        }

        item = item->next;
        ++i;
    }
}

void copyString(char** to, char* from, size_t size,
        size_t maxSize, int endOfFile)
{
    if(endOfFile) ++size;
    if(size > maxSize) size = maxSize;

    (*to) = malloc(size);

    if((*to) != NULL)
    {
        memcpy((*to), from, size);
        (*to)[size - 1] = 0;
    }
}

bool readMenuFromFile(char* fileName, struct SMenuItem** menu, int* numItems)
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
                    copyString(&(item->name), line, len,
                            MAX_NAME_LENGTH, feof(file));
                } else if(item->path == NULL)
                {
                    copyString(&(item->path), line, len,
                        MAX_PATH_LENGTH, feof(file));
                } else
                {
                    copyString(&(item->executable), line, len,
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
                    *numItems = *numItems + 1;
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

void screen(const unsigned op)
{
    union REGS regs;

    switch(op)
    {
        case INT86_SET_TEXT_MODE:
            regs.h.ah = 0x00;
            regs.h.al = 0x03;
            break;

        case INT86_SET_CURSOR_HOME:
            regs.h.ah = 0x02;
            regs.h.bh = 0x00;
            regs.h.dh = 0x00;
            regs.h.dl = 0x00;
            break;

        case INT86_HIDE_CURSOR:
            regs.h.ah = 0x01;
            regs.w.cx = 0x2607;
            break;

        case INT86_CLEAR_SCREEN:
            regs.w.cx = 0;
            regs.w.dx = 0x1850;
            regs.h.bh = 7;
            regs.w.ax = 0x0600;
            break;

        case INT86_RESTORE_CURSOR:
        default:
            regs.h.ah = 0x01;
            regs.w.cx = 0x0607;
            break;
    }

    int86(0x10, &regs, &regs);
}

int checkKey()
{
    union REGS regs;
    regs.h.ah = 0x08;
    int86(0x21, &regs, &regs);
    if(regs.h.al == 0)
    {
        return(checkKey() + 0x100);
    } else
    {
        return(regs.h.al);
    }
}

void updatePosition(bool down, int numItems, int* selected,
        int* first, int* last)
{
    if(down)
    {
        *selected = (*selected) + 1;

        if(*selected > *last)
        {
            (*first)++;
            *last = *selected;
        }

        if(*selected == numItems)
        {
            *selected = 0;
            *first = 0;

            if(numItems < MAX_NUM_MENU_ITEMS)
            {
                *last = numItems - 1;
            } else
            {
                *last = MAX_NUM_MENU_ITEMS - 1;
            }
        }
    } else
    {
        *selected = (*selected) - 1;

        if(*selected < *first)
        {
            (*first)--;
            (*last)--;
        }

        if(*selected < 0)
        {
            *selected = numItems - 1;

            if(numItems >= MAX_NUM_MENU_ITEMS)
            {
                *first = numItems - MAX_NUM_MENU_ITEMS;
            } else
            {
                *first = 0;
            }

           *last = *selected;
        } 
    }
}

bool parseArguments(int argc, char** argv,
        unsigned* fileNameIndex, bool* noAnsi)
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
                *fileNameIndex = i + 1;
                foundFileArg = true;
            } else if(!strcmp(ANSI_OPTION, argv[i]))
            {
                *noAnsi = true;
            }
        }
    }

    if(!foundFileArg)
    {
        printf("ERROR: invalid arguments\n");
    }

    return foundFileArg;
}

void runSelected(int selected, struct SMenuItem* menu, int* errorLevel)
{
    int i;
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
                fprintf(file, "set errorlevel=1\n");
                fprintf(file, "if not exist %s\\%s goto not_exist\n",
                        item->path, item->executable);
                fprintf(file, "cd %s\n", item->path);
                fprintf(file, "call  %s\n", item->executable);
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
                printf("ERROR: file open failed %s\n", RUN_FILE);
            }
        }

        item = item->next;
    }
}

void main(int argc, char* argv[])
{
    struct SMenuItem* menu = NULL;
    int errorLevel = ERRORLEVEL_EXIT_ERROR;
    int numItems = 0;
    int selected = 0;
    int first = 0;
    int last = 0;
    int key = 0;
    bool quit = false;
    unsigned fileNameIndex = 0;
    bool noAnsi = false;
    
    if(parseArguments(argc, argv, &fileNameIndex, &noAnsi) &&
            readMenuFromFile(argv[fileNameIndex], &menu, &numItems))
    {
        screen(INT86_SET_TEXT_MODE);
        screen(INT86_HIDE_CURSOR);
        screen(INT86_SET_CURSOR_HOME);
        
        if(numItems < MAX_NUM_MENU_ITEMS)
        {
            last = numItems - 1;
        } else
        {
            last = MAX_NUM_MENU_ITEMS - 1;
        }

        printMenu(menu, selected, first, last, noAnsi);
    } else
    {
        quit = true;
    }

    while(!quit)
    {
        if(kbhit())
        {
            key = checkKey();

            switch(key)
            {
                case KEY_QUIT:
                    quit = true;
                    errorLevel = ERRORLEVEL_EXIT_ERROR;
                    break;
                case KEY_UP:
                    updatePosition(false, numItems, &selected, &first, &last);
                    break;
                case KEY_DOWN:
                    updatePosition(true, numItems, &selected, &first, &last);
                    break;
                case KEY_SELECT:
                    runSelected(selected, menu, &errorLevel);
                    quit = true;
                    break;
                default:
                    break;
            }


            if(!quit)
            {
                screen(INT86_CLEAR_SCREEN);
                screen(INT86_SET_CURSOR_HOME);
                printMenu(menu, selected, first, last, noAnsi);
            }
        }
    }
    
    // todo: we should free the malloced memory (but when program exits, it's released anyway...)
    
    screen(INT86_RESTORE_CURSOR);
    exit(errorLevel);
}
