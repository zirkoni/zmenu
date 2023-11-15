#pragma once
#include "common.h"
#include <stdbool.h>

struct SMenuItem
{
    char* name;
    char* path;
    char* executable;

    struct SMenuItem* next;
};

bool readMenuFromFile(char* fileName, struct SMenuItem** menu);
void printMenu(struct SMenuItem* menu, int selected, int first, int last, bool noAnsi, bool twoColumns);
void runSelected(index_t selected, struct SMenuItem* menu, int* errorLevel);
