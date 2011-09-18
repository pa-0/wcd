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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef _DISPLAY_H
#define _DISPLAY_H


#ifdef WCD_USECONIO
#  include <conio.h>
#endif

#ifdef WCD_USECURSES
#  ifdef MOUSE_MOVED
#    undef MOUSE_MOVED
#  endif
#  include <curses.h>
#endif

int wcstoutf8(char *mbstr, wchar_t *wcstr, int len);
int utf8towcs(wchar_t *wcstr, char *mbstr, int len);
void wcd_printf( const char* format, ... );
size_t str_columns (char *s);
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
