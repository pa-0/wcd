/*
Copyright (C) 1997-2005 Erwin Waterlander

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef _DISPLAY_H
#define _DISPLAY_H


#ifdef WCD_USECONIO
#  include <conio.h>
#endif

#ifdef WCD_USECURSES
#  include <curses.h>
#endif

void wcd_wprintf( const wchar_t* format, ... );
int str_columns (char *s);
void sort_list(nameset list);
int display_list(nameset list,int perfect, int use_numbers, int use_stdout);
int display_list_conio(nameset list,WcdStack ws, int perfect,int use_numbers);
int display_list_curses(nameset list, WcdStack ws, int perfect,int use_numbers);
int display_list_stdout(nameset list,WcdStack ws, int perfect, int use_stdout);

#define OFFSET 39
#define WCD_MAX_INPSTR 256

#ifdef WCD_USECURSES
void wcd_mvwaddstr(WINDOW *win, int x, int y, char *str);
#  define INPUT_WIN_LEN 80
#endif
#define PAGEOFFSET 38

#define SCROLL_WIN_HEIGHT 22
#define INPUT_WIN_HEIGHT 3

#define WCD_ERR_LIST   -1    /* No choice is made from list of matches */
#define WCD_ERR_CURSES -2    /* Curses can't open terminal */

#define WCD_STDOUT_NO     0 /* no stdout */
#define WCD_STDOUT_NORMAL 1 /* stdout mode */
#define WCD_STDOUT_DUMP   2 /* dump to stdout */

#endif
