/*

matchw.h

Copyright (C) 2011 Erwin Waterlander

This is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License, see the file COPYING.

*/

#ifndef _MATCHW_H
#define _MATCHW_H

#include <wchar.h>
int dd_matchmbs(const char *pattern, const char *string, int ignore_case);
int dd_matchwcs(const wchar_t *pattern, const wchar_t *string, int ignore_case);

#endif /* _MATCHW_H */
