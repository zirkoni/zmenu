#pragma once

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

void screen(const unsigned op);
void setCursorPosition(unsigned short x, unsigned short y);
int checkKey();
