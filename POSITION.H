#pragma once
#include "common.h"
#include <stdbool.h>

void updatePositionUp(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void updatePositionDown(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void updatePositionLeft(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void updatePositionRight(index_t* selected, index_t* first, index_t* last, bool twoColumns);
void jumpToPosition(index_t inputIndex, index_t* selected, index_t* first, index_t* last, bool twoColumns);
