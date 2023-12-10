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
        size_t maxSize, int endOfFile, size_t prefixLen)
{
    if(endOfFile) ++size;
    if((size + prefixLen) > maxSize)
    {
        size = maxSize - prefixLen;
    }

    (*to) = malloc(size);

    if((*to) != NULL)
    {
        memcpy(*to, from, size);
        (*to)[size - 1] = 0;
    } else
    {
        printf("ERROR: malloc failed. Menu too big?\n");
        return false;
    }

    return true;
}

bool copyString(char** to, char* from, size_t size, size_t maxSize, int endOfFile)
{
    return copyStringWithPrefix(to, from, size, maxSize, endOfFile, 0);
}

bool copyName(char** to, char* from, size_t size, int endOfFile)
{
    size_t prefixLen = numDigits(g_numItems) + 2; // dot + space = +2
    return copyStringWithPrefix(to, from, size, g_maxNameLength, endOfFile, prefixLen);
}

