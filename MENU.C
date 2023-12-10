#include "menu.h"
#include "int.h"
#include "copy_str.h"
#include <string.h>

struct SMenuItem* newMenuItem()
{
    struct SMenuItem* item = malloc(sizeof(struct SMenuItem));
    if(item != NULL)
    {
        item->idx = 0;
        item->name = NULL;
        item->path = NULL;
        item->executable = NULL;
        item->next = NULL;
    }

    return item;
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
            printf(" %lu. %s", item->idx, item->name);
        } else
        {
            if(!noAnsi)
            {
                printf(" \033[7m%lu. %s\033[0m", item->idx, item->name);
            } else
            {
                printf("[%lu. %s]", item->idx, item->name);
            }
        }

        item = item->next;
        ++i;
    }
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
                    item->idx = g_numItems;
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

