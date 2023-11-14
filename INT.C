#include "int.h"
#include <dos.h>
#include <i86.h>

void screenDo(const unsigned op, unsigned short arg1, unsigned short arg2)
{
    union REGS regs;

    switch(op)
    {
        case INT86_SET_TEXT_MODE:
            regs.h.ah = 0x00;
            regs.h.al = 0x03;
            break;

        case INT86_SET_CURSOR_POS:
        case INT86_SET_CURSOR_HOME:
            regs.h.ah = 0x02;
            regs.h.bh = 0x00;
            regs.h.dh = arg1;
            regs.h.dl = arg2;
            break;

        case INT86_HIDE_CURSOR:
            regs.h.ah = 0x01;
            regs.w.cx = 0x2607;
            break;

        case INT86_CLEAR_SCREEN:
            regs.w.cx = 0;
            regs.w.dx = 0x1850;
            regs.h.bh = 7;
            regs.w.ax = 0x0600;
            break;

        case INT86_RESTORE_CURSOR:
        default:
            regs.h.ah = 0x01;
            regs.w.cx = 0x0607;
            break;
    }

#if defined(__386__)
    int386(0x10, &regs, &regs);
#else
    int86(0x10, &regs, &regs);
#endif
}

void screen(const unsigned op)
{
    screenDo(op, 0, 0);
}

void setCursorPosition(unsigned short x, unsigned short y)
{
    screenDo(INT86_SET_CURSOR_POS, x, y);
}

int checkKey()
{
    union REGS regs;
    regs.h.ah = 0x08;
#if defined(__386__)
    int386(0x21, &regs, &regs);
#else
    int86(0x21, &regs, &regs);
#endif
    if(regs.h.al == 0)
    {
        return(checkKey() + 0x100);
    } else
    {
        return(regs.h.al);
    }
}

