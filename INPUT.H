#pragma once
#include "common.h"
#include "menu.h"
#include <stdbool.h>

struct ProgramStatus
{
    bool quit;
    bool changeToIndex;
    int errorLevel;
    index_t selected;
    index_t first;
    index_t last;
    index_t inputIndex;
};

bool handleInput(struct ProgramStatus* status, struct SMenuItem* menu, bool columns);
void clearKeyboardBuffer();
