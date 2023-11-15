#include "copy_str.h"
#include "common.h"
#include <string.h>

size_t g_maxNameLength = MAX_NAME_LENGTH_1;

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

bool copyString(char** to, char* from, size_t size, size_t maxSize, int endOfFile)
{
    return copyStringWithPrefix(to, from, size, maxSize, endOfFile, false, 0);
}

bool copyName(char** to, char* from, size_t size, int endOfFile)
{
    return copyStringWithPrefix(to, from, size, g_maxNameLength, endOfFile, true, g_numItems);
}

