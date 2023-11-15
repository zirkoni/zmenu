#pragma once
#include <stdio.h>

#define KEY_SELECT     13
#define KEY_QUIT       27
#define KEY_UP        328
#define KEY_DOWN      336
#define KEY_LEFT      331
#define KEY_RIGHT     333
#define KEY_BACKSPACE   8

#define INT86_SET_TEXT_MODE   0
#define INT86_SET_CURSOR_HOME 1
#define INT86_HIDE_CURSOR     2
#define INT86_CLEAR_SCREEN    3
#define INT86_RESTORE_CURSOR  4
#define INT86_SET_CURSOR_POS  5

#define MAX_NUM_ITEMS_ON_SCREEN_1 23
#define MAX_NUM_ITEMS_ON_SCREEN_2 46

#define MAX_NAME_LENGTH_1         60
#define MAX_NAME_LENGTH_2         38
#define MAX_PATH_LENGTH           36
#define MAX_EXE_LENGTH            12

#define ERRORLEVEL_CONTINUE   0
#define ERRORLEVEL_EXIT_ERROR 1

typedef long unsigned index_t;

extern index_t g_numItems;
extern size_t g_maxNumItemsOnScreen;
extern size_t g_maxNameLength;
