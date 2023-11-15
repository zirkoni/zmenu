#include "position.h"

index_t g_numItems = 0;
size_t g_maxNumItemsOnScreen = MAX_NUM_ITEMS_ON_SCREEN_1;

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

