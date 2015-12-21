/*

matchw.h

Copyright (C) 2011 Erwin Waterlander

This is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License, see the file COPYING.

*/

#ifndef _MATCHW_H
#define _MATCHW_H

#include <wchar.h>
int dd_matchmbs(const char *string, const char *pattern, int ignore_case, int ignore_diacritics);
int dd_matchwcs(const wchar_t *string, const wchar_t *pattern, int ignore_case, int ignore_diacritics);

#endif /* _MATCHW_H */
