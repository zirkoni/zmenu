#pragma once
#include <stdbool.h>
#include <stdio.h> // for size_t

#define MAX_NUM_ITEMS_ON_SCREEN_1 23
#define MAX_NUM_ITEMS_ON_SCREEN_2 46

typedef long unsigned index_t;

extern index_t g_numItems;
extern size_t g_maxNumItemsOnScreen;

void updatePositionUp(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void updatePositionDown(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void updatePositionLeft(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void updatePositionRight(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void jumpToPosition(index_t inputIndex, index_t* selected, index_t* first, index_t* last, bool twoColumns);
