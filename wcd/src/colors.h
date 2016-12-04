/*------------------------------------------------------------------*\
| This source code is copied from PDCurses 3.1 demos\tui.c and tui.h |
|                                                                    |
| Author    : P.J. Kunst                                             |
| Date      : 25-02-93                                               |
| colors.h is in the public domain                                   |
|                                                                    |
|                                                                    |
| Billy Chen                          12:49  Monday, 4 June, 2007    |
\*------------------------------------------------------------------*/

#ifndef _WCD_COLORS_H
#define _WCD_COLORS_H

#ifdef A_COLOR
# define MENU_COLOR          (2 | A_BOLD)
# define MENU_SELECT         (4 | A_BOLD)
# define BODY_COLOR          6
#else
# define MENU_COLOR          (A_BOLD)
# define MENU_SELECT         (A_BOLD)
# define BODY_COLOR          0
#endif

#ifdef A_COLOR
#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR)  /* A_BLINK, A_REVERSE, A_BOLD */
#else
#define A_ATTR  (A_ATTRIBUTES)            /* standard UNIX attributes */
#endif

#define MAXSTRLEN  256
#define KEY_ESC    0x1b     /* Escape */

void initcolor (void);
void setcolor (WINDOW *, chtype color);
void colorbox (WINDOW *, chtype color, int hasbox);

#endif
