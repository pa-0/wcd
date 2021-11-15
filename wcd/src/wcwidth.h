/*
Copyright (C) 2013 Erwin Waterlander

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

#ifndef _WCD_WCWIDTH_H
#define _WCD_WCWIDTH_H

#include <wchar.h>

int mk_wcwidth(wchar_t ucs);
int mk_wcswidth(const wchar_t *pwcs, size_t n);
int mk_wcwidth_cjk(wchar_t ucs);
int mk_wcswidth_cjk(const wchar_t *pwcs, size_t n);
#if defined (_WIN32) && !defined(__CYGWIN__)
/* Watccom C, MS-VS, and MinGW don't have wc(s)width().
   MinGW-w64 has wc(s)width(). */
int wcwidth(wchar_t ucs);
int wcswidth(const wchar_t *pwcs, size_t n);
#endif

#endif
