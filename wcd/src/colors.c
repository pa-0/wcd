/*------------------------------------------------------------------*\
| This source code is copied from PDCurses 3.1 demos\tui.c and tui.h |
|                                                                    |
| Author    : P.J. Kunst                                             |
| Date      : 25-02-93                                               |
| colors.c is in the public domain                                   |
|                                                                    |
|                                                                    |
| Billy Chen                          12:49  Monday, 4 June, 2007    |
\*------------------------------------------------------------------*/


#ifdef WCD_USECURSES

#include <curses.h>
/* MOUSE_MOVED in PDCurses' curses.h conflicts with MOUSE_MOVED in wincon.h */
#ifdef MOUSE_MOVED
#  undef MOUSE_MOVED
#endif
#include "colors.h"
#include "display.h"

void initcolor(void)
{
#ifdef A_COLOR
    if (has_colors())
        start_color();

    /* foreground, background */
#if 1
    init_pair((short)(MENU_COLOR      & ~A_ATTR), (short)COLOR_WHITE,   (short)COLOR_BLUE);
    init_pair((short)(BODY_COLOR      & ~A_ATTR), (short)COLOR_WHITE,   (short)COLOR_RED);
#else
    init_pair((short)(MENU_COLOR      & ~A_ATTR), (short)COLOR_WHITE,   (short)COLOR_RED);
    init_pair((short)(BODY_COLOR      & ~A_ATTR), (short)COLOR_CYAN,    (short)COLOR_BLUE);
#endif

#endif
}

void setcolor(WINDOW *win, chtype color)
{
    chtype attr = color & A_ATTR;  /* extract Bold, Reverse, Blink bits */

#ifdef A_COLOR
    attr &= ~A_REVERSE;  /* ignore reverse, use colors instead! */
    wattrset(win, COLOR_PAIR(color & A_CHARTEXT) | attr);
#else
    attr &= ~A_BOLD;     /* ignore bold, gives messy display on HP-UX */
    wattrset(win, attr);
#endif
}

void colorbox(WINDOW *win, chtype color, int hasbox)
{
    int maxy;
#ifndef PDCURSES
    int maxx;
#endif
    chtype attr = color & A_ATTR;  /* extract Bold, Reverse, Blink bits */

    setcolor(win, color);

#ifdef A_COLOR
    if (has_colors())
        wbkgd(win,
            COLOR_PAIR(color & A_CHARTEXT) | (attr & ~A_REVERSE));
    else
#endif
        wbkgd(win, color);

    WCD_WCLEAR(win);

#ifdef PDCURSES
    maxy = getmaxy(win);
#else
    getmaxyx(win, maxy, maxx);
#endif
    if (hasbox && (maxy > 2))
        box(win, (chtype)0, (chtype)0);

    touchwin(win);
    wrefresh(win);
}

#endif /* WCD_USECURSES */

