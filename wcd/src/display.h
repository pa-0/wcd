/*
Copyright (C) 1997-2015 Erwin Waterlander

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
#  include <curses.h>
/* MOUSE_MOVED in PDCurses' curses.h conflicts with MOUSE_MOVED in wincon.h */
#  ifdef MOUSE_MOVED
#    undef MOUSE_MOVED
#  endif
#endif

#if (defined(WCD_ANSI) || defined(WCD_UTF16))
#include <wchar.h>
#endif
#include "std_macr.h"
#include "structur.h"

#ifdef WCD_ANSI
size_t wcstoansi(char *mbstr, const wchar_t *wcstr, int len);
size_t ansitowcs(wchar_t *wcstr, const char *mbstr, int len);
#endif
#ifdef WCD_UTF16
size_t wcstoutf8(char *mbstr, const wchar_t *wcstr, int len);
size_t utf8towcs(wchar_t *wcstr, const char *mbstr, int len);
#endif
void wcd_printf( const char* format, ... );
#if (defined(WCD_USECONIO) || defined(WCD_USECURSES))
size_t str_columns (char *s);
#endif
void sort_list(nameset list);
int display_list(nameset list,int perfect, int use_numbers, int use_stdout);
#ifdef WCD_USECONIO
int display_list_conio(nameset list,WcdStack ws, int perfect,int use_numbers);
#endif
#ifdef WCD_USECURSES
int display_list_curses(nameset list, WcdStack ws, int perfect,int use_numbers);
void wcd_mvwaddstr(WINDOW *win, int x, int y, char *str);
#endif
int display_list_stdout(nameset list,WcdStack ws, int perfect, int use_stdout);

#define OFFSET 39
#define WCD_MAX_INPSTR 256

#define PAGEOFFSET 38

#define SCROLL_WIN_HEIGHT 22
#define INPUT_WIN_HEIGHT 3

#define WCD_ERR_LIST   -1    /* No choice is made from list of matches */
#define WCD_ERR_CURSES -2    /* Curses can't open terminal */

#define WCD_STDOUT_NO     0 /* no stdout */
#define WCD_STDOUT_NORMAL 1 /* stdout mode */
#define WCD_STDOUT_DUMP   2 /* dump to stdout */

#if defined(_WIN32) && !defined(__CYGWIN__) /* Windows, not Cygwin */
/* On Windows we use wclear, because wclear gives a cleaner screen in a Windows
 * Command Prompt in an East Asian locale with double width font. E.g. Chinese
 * locale CP936 with raster or Simsun font. wclear gives some screen
 * flickering when used with ncurses on Windows. */
#define WCD_WCLEAR wclear
#else
/* We prefer werase, because it gives a steadier screen. Less flickering. */
#define WCD_WCLEAR werase
#endif

#endif
