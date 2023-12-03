#include "position.h"

index_t g_numItems = 0;
size_t g_maxNumItemsOnScreen = MAX_NUM_ITEMS_ON_SCREEN_1;

void updatePositionUp(index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    const unsigned step = 1 + twoColumns;

    if(*selected >= step)
    {
        *selected = *selected - step;

        // Scroll
        if(*selected < *first)
        {
            *first = *first - step;

            // If 1st column longer than 2nd, last item is visible
            if(twoColumns && *last == (g_numItems - 1) && (*last % 2) == 0)
            {
                *last = *last - 1;
            } else
            {
                *last = *last - step;
            }
        }
    } else
    {
        if(twoColumns)
        {
            *last = g_numItems - 1;

            if(*selected % 2) // 2nd column
            {
                if(g_numItems % 2) // 1st column is longer than 2nd
                {
                    *selected = g_numItems - 2;
                    *first = *last - g_maxNumItemsOnScreen + 2;
                } else
                {
                    *selected = g_numItems - 1;
                    *first = *last - g_maxNumItemsOnScreen + 1;
                }
            } else // 1st column
            {
                if(g_numItems % 2) // 1st column is longer
                {
                    *selected = g_numItems - 1;
                    *first = *last - g_maxNumItemsOnScreen + 2;
                } else
                {
                    *selected = g_numItems - 2;
                    *first = *last - g_maxNumItemsOnScreen + 1;
                }
            }

            // 1st is always 0 for small menu
            if(g_numItems < g_maxNumItemsOnScreen)
            {
                *first = 0;
            }

        } else
        {
            *selected = g_numItems - 1;

            if(g_numItems >= g_maxNumItemsOnScreen)
            {
                *first = g_numItems - g_maxNumItemsOnScreen;
            } else
            {
                *first = 0;
            }

            *last = *selected;
        }
    }
}

void updatePositionDown(index_t* selected, index_t* first, index_t* last, bool twoColumns)
{
    const int step = 1 + twoColumns;
    *selected = (*selected) + step;

    if(*selected >= g_numItems)
    {
        // Return to beginning
        if(!twoColumns || !(*selected % 2))
        {
            *selected = 0;
        } else
        {
            *selected = 1;
        }

        *first = 0;

        if(g_numItems > g_maxNumItemsOnScreen)
        {
            *last = g_maxNumItemsOnScreen - 1;
        } else
        {
            *last = g_numItems - 1;
        }
    } else if(*selected > *last)
    {
        // Scroll
        *first += step;
        *last += step;

        if(*last >= g_numItems)
        {
            *last = g_numItems - 1;
        }
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

